#include <Arduino.h>
#include <HomingEncoder.h>
#include <Streaming.h>

#include <pwm_lib.h>
using namespace arduino_due::pwm_lib;

HomingEncoder hEnc;

#define DRIVER_LEFT_PIN_1 4
#define DRIVER_LEFT_PIN_2 5
#define DRIVER_LEFT_PIN_PWM 6

#define ENCODER_LEFT_PIN_1 2
#define ENCODER_LEFT_PIN_2 3
#define BREAKER_LEFT_PIN A0

pwm<pwm_pin::PWML7_PC24> driverLeftPinPWM;

void setup() {
  Serial.begin(9600);
  Serial << "Hello Playground!" << endl; 

  hEnc.init<0>(ENCODER_LEFT_PIN_1, ENCODER_LEFT_PIN_2, BREAKER_LEFT_PIN );

  pinMode( DRIVER_LEFT_PIN_1, OUTPUT );
  pinMode( DRIVER_LEFT_PIN_2, OUTPUT );
  pinMode( DRIVER_LEFT_PIN_PWM, OUTPUT );

  digitalWrite( DRIVER_LEFT_PIN_1, HIGH );
  digitalWrite( DRIVER_LEFT_PIN_2, LOW );
  driverLeftPinPWM.set_period_and_duty(5000,1500);
}

void loop() {
    delay(500);
    hEnc.printStatus();
}