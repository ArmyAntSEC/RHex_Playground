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
};

class HomingEncoder
{
    private:
        HomingEncoderState state;
    
    public:        
        void init(unsigned int encoderPin1, unsigned int encoderPin2, unsigned int breakerPin )
        {
            state.encoderPin1 = encoderPin1;
            state.encoderPin2 = encoderPin2;
            state.breakerPin = breakerPin;
            state.count_encoder = 0;
            state.count_homing = 0;    

            if ( encoderCount >= MAX_ENCODERS_SUPPORTED )
                Serial << "ERROR: More encoders registered than are supported." << endl;
            stateList[encoderCount] = &state;

            pinMode(encoderPin1, INPUT);
            pinMode(encoderPin2, INPUT);
            pinMode(breakerPin, INPUT);

            attach( encoderCount, encoderPin1, encoderPin2, breakerPin );
        }

        void printStatus()
        {
            Serial << "Count Encoder: " << stateList[0]->count_encoder << 
                " Count Homing: " << stateList[0]->count_homing << endl;    

        }
    
    public:    
        static HomingEncoderState * stateList[MAX_ENCODERS_SUPPORTED];
        static int encoderCount;
    
    public:
        static void update( HomingEncoderState* state )
        {
            state->count_encoder++;
        }

        static void home( HomingEncoderState* state )
        {
            state->count_homing++;
        }

        static void attach ( unsigned int count, unsigned int encoderPin1,
            unsigned int encoderPin2, unsigned int breakerPin )
        {
            switch( count ) {
                case 0:
                    attachInterrupt(digitalPinToInterrupt(encoderPin1), isr0, CHANGE );
                    attachInterrupt(digitalPinToInterrupt(encoderPin2), isr0, CHANGE );
                    attachInterrupt(digitalPinToInterrupt(breakerPin), isr0_homing, FALLING );
                    break;
                default:
                    Serial << "ERROR: Tried to register more encoders than are supported" << endl;            
            }
        }

    public:
        static void isr0(void) { update(stateList[0]); }
        static void isr0_homing(void) { home(stateList[0]); }

};

#endif