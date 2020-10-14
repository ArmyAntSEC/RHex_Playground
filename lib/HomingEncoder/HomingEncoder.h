#ifndef _HOMINGENCODER_H_
#define _HOMINGENCODER_H_

#include <Arduino.h>

#define MAX_ENCODERS_SUPPORTED 6

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
        void init(unsigned int encoderPin1, unsigned int encoderPin2, unsigned int breakerPin );
        void printStatus();
        static HomingEncoderState * stateList[MAX_ENCODERS_SUPPORTED];
        static int encoderCount;
    public:
        static void update( HomingEncoderState* state );
        static void home( HomingEncoderState* state );
        static void attach ( unsigned int count, unsigned int encoderPin1,
            unsigned int encoderPin2, unsigned int breakerPin );
    public:
        static void isr0(void) { update(stateList[0]); }
        static void isr0_homing(void) { home(stateList[0]); }

};

#endif