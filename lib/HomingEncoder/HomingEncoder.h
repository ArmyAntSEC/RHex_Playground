/* Homing Encoder Library for Arduino Due
 * Copyright 2020 Daniel Armyr
 * 
 * The core algorithm, some macros and lines of code were taken from:
 * Encoder Library, for measuring quadrature encoded signals
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 * Copyright (c) 2011,2013 PJRC.COM, LLC - Paul Stoffregen <paul@pjrc.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _HOMINGENCODER_H_
#define _HOMINGENCODER_H_

#include <Arduino.h>
#include <Streaming.h>

#define IO_REG_TYPE			uint32_t
#define PIN_TO_BASEREG(pin)             (portInputRegister(digitalPinToPort(pin)))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define DIRECT_PIN_READ(base, mask)     (((*(base)) & (mask)) ? 1 : 0)

#define MAX_ENCODERS_SUPPORTED 6

#define ENCODER_LEFT_PIN_1 2
#define ENCODER_LEFT_PIN_2 3
#define BREAKER_LEFT_PIN A0

#define LOG Serial << "HomingEncoder: "

struct HomingEncoderState {
    int encoderPin1;
    int encoderPin2;
    int breakerPin;

    volatile IO_REG_TYPE * encoderPin1_register;
	volatile IO_REG_TYPE * encoderPin2_register;
    volatile IO_REG_TYPE * breakerPin_register;

	IO_REG_TYPE            encoderPin1_bitmask;
	IO_REG_TYPE            encoderPin2_bitmask;
    IO_REG_TYPE            breakerPin_bitmask;

	uint8_t                encoder_state;
    bool moving_forward;

    long int position;

    int count_encoder;
    int count_homing;  
    int last_count_at_homing;  
};

class HomingEncoder
{
    private:
        HomingEncoderState state;
    
    public:     
        static HomingEncoderState * stateList[MAX_ENCODERS_SUPPORTED];        

        template <int N> void init( unsigned int encoderPin1, unsigned int encoderPin2, unsigned int breakerPin )
        {
            pinMode(encoderPin1, INPUT_PULLUP);
            pinMode(encoderPin2, INPUT_PULLUP);
            pinMode(breakerPin, INPUT_PULLUP);

            state.encoderPin1 = encoderPin1;
            state.encoderPin2 = encoderPin2;
            state.breakerPin = breakerPin;
            state.count_encoder = 0;
            state.count_homing = 0;    
            state.last_count_at_homing = 0;
            state.position = 0;
            state.moving_forward = true;

            state.encoderPin1_register = PIN_TO_BASEREG(encoderPin1);
            state.encoderPin2_register = PIN_TO_BASEREG(encoderPin2);
            state.breakerPin_register = PIN_TO_BASEREG(breakerPin);

            state.encoderPin1_bitmask = PIN_TO_BITMASK(encoderPin1);
            state.encoderPin2_bitmask = PIN_TO_BITMASK(encoderPin2);
            state.breakerPin_bitmask = PIN_TO_BITMASK(breakerPin);

            // allow time for a passive R-C filter to charge
		    // through the pullup resistors, before reading
		    // the initial state
		    delayMicroseconds(2000);

            uint8_t s = 0;
		    if (DIRECT_PIN_READ(state.encoderPin1_register, state.encoderPin1_bitmask)) s |= 1;
		    if (DIRECT_PIN_READ(state.encoderPin1_register, state.encoderPin1_bitmask)) s |= 2;
		    
            state.encoder_state = s;

            LOG << "Starting encoder state: " << state.encoder_state << endl;

            if ( N >= MAX_ENCODERS_SUPPORTED ) {
                LOG << "ERROR: More encoders registered than are supported." << endl;
            } else {
                stateList[N] = &state;
                
                attachInterrupt(digitalPinToInterrupt(encoderPin1), isr_encoder<N>, CHANGE );
                attachInterrupt(digitalPinToInterrupt(encoderPin2), isr_encoder<N>, CHANGE );
                attachInterrupt(digitalPinToInterrupt(breakerPin), isr_homing<N>, CHANGE );                     

            }
        }

        void printStatus()        
        {
            LOG << " Position: " << stateList[0]->position <<    
                " Count Encoder: " << stateList[0]->count_encoder << 
                " Count Homing: " << stateList[0]->count_homing << 
                " Count at last homing: " << stateList[0]->last_count_at_homing << 
                " Direction: " << stateList[0]->moving_forward <<                 
                endl;                
        }
    
        template<int N> static void isr_encoder(void) 
        {
            HomingEncoderState * state = stateList[N];

            uint8_t p1val = DIRECT_PIN_READ(state->encoderPin1_register, 
                state->encoderPin1_bitmask );
		    uint8_t p2val = DIRECT_PIN_READ(state->encoderPin2_register,
                state->encoderPin2_bitmask );
		    
            uint8_t encoder_state = state->encoder_state & 3;
		    if (p1val) encoder_state |= 4;
		    if (p2val) encoder_state |= 8;
		    state->encoder_state = (encoder_state >> 2);

            state->count_encoder++;

            switch (encoder_state) {
                case 1: case 7: case 8: case 14:
                    state->position--;
                    state->moving_forward = false;
                    break;
                case 2: case 4: case 11: case 13:
                    state->position++;
                    state->moving_forward = true;
                    break;
                case 3: case 12:
                    state->position -= 2;
                    state->moving_forward = false;
                    break;
                case 6: case 9:
                    state->position += 2;
                    state->moving_forward = true;
                    break;
            }                                    
        }

        template<int N> static void isr_homing(void) 
        { 
            HomingEncoderState * state = stateList[N];
            
            uint8_t breaker_val = DIRECT_PIN_READ(state->breakerPin_register, 
                state->breakerPin_bitmask );                  
                    
            //Depending on direction, we will trigger either on rising or falling. 
            //We want to make sure we allways trigger on the same edge regardless of direction
            if ( state->moving_forward ^ breaker_val ) {
                state->count_homing++;            
                state->last_count_at_homing = state->count_encoder;
                state->count_encoder = 0;
                state->position = 0;
            }            
        }
};

#endif