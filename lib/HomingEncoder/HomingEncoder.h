#ifndef _HOMINGENCODER_H_
#define _HOMINGENCODER_H_

#include <Arduino.h>
#include <Streaming.h>

#define MAX_ENCODERS_SUPPORTED 6

#define ENCODER_LEFT_PIN_1 2
#define ENCODER_LEFT_PIN_2 3
#define BREAKER_LEFT_PIN A0

struct HomingEncoderState {
    int encoderPin1;
    int encoderPin2;
    int breakerPin;

    int count_encoder;
    int count_homing;  
    int last_count_at_homing;  
};

class HomingEncoder
{
    private:
        HomingEncoderState state;
    
    public:     
        template <int N> void init(  unsigned int encoderPin1, unsigned int encoderPin2, unsigned int breakerPin )
        {
            state.encoderPin1 = encoderPin1;
            state.encoderPin2 = encoderPin2;
            state.breakerPin = breakerPin;
            state.count_encoder = 0;
            state.count_homing = 0;    
            state.last_count_at_homing = 0;

            if ( N >= MAX_ENCODERS_SUPPORTED )
                Serial << "ERROR: More encoders registered than are supported." << endl;
            stateList[N] = &state;

            pinMode(encoderPin1, INPUT);
            pinMode(encoderPin2, INPUT);
            pinMode(breakerPin, INPUT);

            attach<N>( encoderPin1, encoderPin2, breakerPin );
        }

        void printStatus()
        {
            Serial << "Count Encoder: " << stateList[0]->count_encoder << 
                " Count Homing: " << stateList[0]->count_homing << 
                " Count at last homing: " << stateList[0]->last_count_at_homing << endl;    

        }
    
    public:    
        static HomingEncoderState * stateList[MAX_ENCODERS_SUPPORTED];        
    
    public:

        template<int N> static void attach ( unsigned int encoderPin1,
            unsigned int encoderPin2, unsigned int breakerPin )
        {         
            if ( N >= MAX_ENCODERS_SUPPORTED )
                Serial << "ERROR: Tried to register more encoders than supported. Ignoring." << endl;
            else {
                attachInterrupt(digitalPinToInterrupt(encoderPin1), isr_encoder<N>, CHANGE );
                attachInterrupt(digitalPinToInterrupt(encoderPin2), isr_encoder<N>, CHANGE );
                attachInterrupt(digitalPinToInterrupt(breakerPin), isr_homing<N>, FALLING );                     
            }
        }

    public:
        template<int N> static void isr_encoder(void) 
        {
            HomingEncoderState * state = stateList[N];
            state->count_encoder++;
        }

        template<int N> static void isr_homing(void) 
        { 
            HomingEncoderState * state = stateList[N];
            state->count_homing++;            
            state->last_count_at_homing  = state->count_encoder;
            state->count_encoder = 0;
        }

};

#endif