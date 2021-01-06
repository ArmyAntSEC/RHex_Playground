#include <Arduino.h>
#include <HomingEncoder.h>
#include <Streaming.h>

#include <pwm_lib.h>
using namespace arduino_due::pwm_lib;

HomingEncoder hEnc;

#define DRIVER_LEFT_PIN_1 5
#define DRIVER_LEFT_PIN_2 6
#define DRIVER_LEFT_PIN_PWM 7
#define DRIVER_LEFT_PIN_CS A0

#define ENCODER_LEFT_PIN_1 3
#define ENCODER_LEFT_PIN_2 4
#define BREAKER_LEFT_PIN 2


pwm<pwm_pin::PWML7_PC24> driverLeftPinPWM;

unsigned long int start_time = 0;
unsigned int driver_pwm = 1500;

void setup() {

  start_time = millis();

  Serial.begin(9600);
  Serial << "Hello Playground!" << endl; 

  //hEnc.init<0>(ENCODER_LEFT_PIN_1, ENCODER_LEFT_PIN_2, BREAKER_LEFT_PIN );

  pinMode( DRIVER_LEFT_PIN_1, OUTPUT );
  pinMode( DRIVER_LEFT_PIN_2, OUTPUT );
  pinMode( DRIVER_LEFT_PIN_PWM, OUTPUT );

  digitalWrite( DRIVER_LEFT_PIN_1, LOW );
  digitalWrite( DRIVER_LEFT_PIN_2, HIGH );
  if ( driverLeftPinPWM.start(5000,driver_pwm) )
    Serial << "PWM started successfully." << endl;
  else
    Serial << "Issue starting PWM" << endl;
}

bool has_stopped = false;
void loop() {
  if ( millis() < start_time + 1000 ) {
    delay(200);
    //driver_pwm += 250;
    //driverLeftPinPWM.set_duty(driver_pwm);
    //Serial << "PWM: " << driver_pwm << endl;
    //Serial << "Position: " << hEnc.read() << " Is Homed? " << hEnc.isHomed() << endl;
  } else if ( !has_stopped ) {
    driverLeftPinPWM.stop();
    Serial << "Stopped" << endl;
    has_stopped = true;
  }
}