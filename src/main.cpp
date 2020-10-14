#include <Arduino.h>
#include <HomingEncoder.h>
#include <Streaming.h>

HomingEncoder hEnc;

#define DRIVER_LEFT_PIN_1 4
#define DRIVER_LEFT_PIN_2 5
#define DRIVER_LEFT_PIN_PWM 6

#define ENCODER_LEFT_PIN_1 2
#define ENCODER_LEFT_PIN_2 3
#define BREAKER_LEFT_PIN A0

void setup() {
  Serial.begin(9600);
  Serial << "Hello Playground!" << endl; 

  hEnc.init(ENCODER_LEFT_PIN_1, ENCODER_LEFT_PIN_2, BREAKER_LEFT_PIN );

  pinMode( DRIVER_LEFT_PIN_1, OUTPUT );
  pinMode( DRIVER_LEFT_PIN_2, OUTPUT );
  pinMode( DRIVER_LEFT_PIN_PWM, OUTPUT );

  digitalWrite( DRIVER_LEFT_PIN_1, HIGH );
  digitalWrite( DRIVER_LEFT_PIN_2, LOW );
  analogWrite( DRIVER_LEFT_PIN_PWM, 64 );
}

void loop() {
    delay(500);
    hEnc.printStatus();
}