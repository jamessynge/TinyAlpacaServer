// A modification of Alan's demo of the cover-calibrator hardware to deal with
// my remapping of the pins.

#include <AccelStepper.h>
#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "utils/avr_timer_counter.h"

// Modified pin selection to avoid pins used for other purposes.
// Pins to avoid:
// D00 - RX (Serial over USB)
// D01 - TX (Serial over USB)
// D04 - SDcard Chip Select
// D10 - W5500 Chip Select
// D13 - Built-in LED
// D50 - MISO (SPI)
// D51 - MOSI (SPI)
// D52 - SCK (SPI)

#define kLedChannel1PwmPin 46      // OC5A      was 5
#define kLedChannel1EnabledPin 43  // PL6       was 9

#define kLedChannel2PwmPin 45      // OC5B      was 6
#define kLedChannel2EnabledPin 41  // PG0       was 10

#define kLedChannel3PwmPin 44      // OC5C      was 7
#define kLedChannel3EnabledPin 39  // PG2       was 11

#define kLedChannel4PwmPin 12      // OC1B      was 8
#define kLedChannel4EnabledPin 37  // A8        was 12

#define kCoverMotorStepPin 3       //
#define kCoverMotorDirectionPin 5  //
#define kCoverOpenLimitPin 6       // PCINT8    was 20
#define kCoverCloseLimitPin 11     // PCINT5    was 21
#define kCoverEnabledPin 42        //           was 13

using ::alpaca::TimerCounter1Pwm16Output;
using ::alpaca::TimerCounter5Pwm16Output;
using ::alpaca::TimerCounterChannel;

TimerCounter5Pwm16Output led1(TimerCounterChannel::A, kLedChannel1EnabledPin);
TimerCounter5Pwm16Output led2(TimerCounterChannel::B, kLedChannel2EnabledPin);
TimerCounter5Pwm16Output led3(TimerCounterChannel::C, kLedChannel3EnabledPin);
TimerCounter1Pwm16Output led4(TimerCounterChannel::B, kLedChannel4EnabledPin);

void blinkPin(int pin, int blink_count) {
  for (int i = 0; i < blink_count; ++i) {
    digitalWrite(pin, HIGH);
    TAS_VLOG(1) << "pin "<< pin<<" set HIGH";
    delay(300);
    digitalWrite(pin, LOW);
    TAS_VLOG(1) << "pin "<< pin<<" set LOW";
    delay(300);
  }
}



void setup() {
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read. Note that the baud rate is meaningful on boards that do true serial,
  // while those microcontrollers with builtin USB likely don't rate limit
  // because there isn't a need.
  Serial.begin(57600);

  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost. Note that this isn't true for all
  // Arduino-like boards: some reset when the Serial Monitor connects, so we
  // almost always get the initial output.
  while (!Serial) {
  }

  TAS_VLOG(1) << "\n\nSerial working\n";

  Serial << "TCCR1A = 0b";
  Serial.println(TCCR1A, 2); 
  Serial << "TCCR1B = 0b";
  Serial.println(TCCR1B, 2); 

  Serial << "TCCR5A = 0b";
  Serial.println(TCCR5A, 2); 
  Serial << "TCCR5B = 0b";
  Serial.println(TCCR5B, 2); 
  
  TAS_VLOG(1) << "Initializing 16 PWM";

  TimerCounter1Initialize16BitFastPwm(alpaca::ClockPrescaling::kAsIs);
  TimerCounter5Initialize16BitFastPwm(alpaca::ClockPrescaling::kAsIs);

  Serial << "TCCR1A = 0b";
  Serial.println(TCCR1A, 2); 
  Serial << "TCCR1B = 0b";
  Serial.println(TCCR1B, 2); 

  Serial << "TCCR5A = 0b";
  Serial.println(TCCR5A, 2); 
  Serial << "TCCR5B = 0b";
  Serial.println(TCCR5B, 2); 

  blinkPin(46, 1000);



  
  blinkPin(13, 20);
  blinkPin(46, 15);
  blinkPin(45, 10);
  blinkPin(44, 20);
}

template <class T>
void Sweep16BitPwm(T& t, uint16_t increment, MillisT delay_by,
                   const char* name) {
  if (t.is_enabled()) {
    TAS_VLOG(1) << name << " is enabled.";
    TAS_VLOG(1) << "Sweeping " << name << " upwards";
    int32_t value = 1;
    while (value <= t.max_count()) {
      t.set_pulse_count(static_cast<uint16_t>(value & 0xFFFF));
      value += increment;
      delay(delay_by);
    }
    TAS_VLOG(1) << "Sweeping " << name << " downwards";
    value = t.max_count();
    while (value >= 1) {
      t.set_pulse_count(static_cast<uint16_t>(value & 0xFFFF));
      value -= increment;
      delay(delay_by);
    }
    TAS_VLOG(1) << "Turning " << name << " off";
    t.set_pulse_count(0);
  } else {
    TAS_VLOG(1) << name << " is disabled.";
  }
}

void loop() {
  Sweep16BitPwm(led1, 50, 4, "led1");
  Sweep16BitPwm(led2, 1, 1, "led2");
  Sweep16BitPwm(led3, 1, 4, "led3");
  Sweep16BitPwm(led4, 1, 4, "led4");
}

#if 0
// Define stepper motor connections and motor interface type. Motor interface
// type must be set to 1 when using a driver:
#define dirPin 4
#define stepPin 3
#define motorInterfaceType 1

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

int led5Pin = 5;  // LED5 connected to digital pin 5
int led6Pin = 6;  // LED6 connected to digital pin 6
int led7Pin = 7;  // LED7 connected to digital pin 7
int led8Pin = 8;  // LED8 connected to digital pin 8

void setup() {
  // Set the maximum speed in steps per second:
  stepper.setMaxSpeed(20000);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}

void loop() {
  // fade in from min to max in increments of 1 ADU:
  for (int fadeValue = 0; fadeValue <= 255; fadeValue += 1) {
    // sets the value (range from 0 to 255):
    analogWrite(led5Pin, fadeValue);
    // wait for 2 milliseconds to see the dimming effect
    delay(2);
  }

  // fade out from max to min in increments of 1 ADU:
  for (int fadeValue = 255; fadeValue >= 0; fadeValue -= 1) {
    // sets the value (range from 0 to 255):
    analogWrite(led5Pin, fadeValue);
    // wait for 2 milliseconds to see the dimming effect
    delay(2);
  }
  // fade in from min to max in increments of 1 ADU:
  for (int fadeValue = 0; fadeValue <= 255; fadeValue += 1) {
    // sets the value (range from 0 to 255):
    analogWrite(led6Pin, fadeValue);
    // wait for 2 milliseconds to see the dimming effect
    delay(2);
  }

  // fade out from max to min in increments of 1 ADU:
  for (int fadeValue = 255; fadeValue >= 0; fadeValue -= 1) {
    // sets the value (range from 0 to 255):
    analogWrite(led6Pin, fadeValue);
    // wait for 2 milliseconds to see the dimming effect
    delay(2);
  }
  // fade in from min to max in increments of 1 ADU:
  for (int fadeValue = 0; fadeValue <= 255; fadeValue += 1) {
    // sets the value (range from 0 to 255):
    analogWrite(led7Pin, fadeValue);
    // wait for 2 milliseconds to see the dimming effect
    delay(2);
  }

  // fade out from max to min in increments of 1 ADU:
  for (int fadeValue = 255; fadeValue >= 0; fadeValue -= 1) {
    // sets the value (range from 0 to 255):
    analogWrite(led7Pin, fadeValue);
    // wait for 2 milliseconds to see the dimming effect
    delay(2);
  }
  // fade in from min to max in increments of 1 ADU:
  for (int fadeValue = 0; fadeValue <= 255; fadeValue += 1) {
    // sets the value (range from 0 to 255):
    analogWrite(led8Pin, fadeValue);
    // wait for 2 milliseconds to see the dimming effect
    delay(2);
  }

  // fade out from max to min in increments of 1 ADU:
  for (int fadeValue = 255; fadeValue >= 0; fadeValue -= 1) {
    // sets the value (range from 0 to 255):
    analogWrite(led8Pin, fadeValue);
    // wait for 2 milliseconds to see the dimming effect
    delay(2);
  }
  // Set the current position to 0:
  stepper.setCurrentPosition(0);

  // Run the motor forward at 10000 steps/second until the motor reaches 108000
  // steps (3/4 revolutions):

  while (stepper.currentPosition() != 108000) {
    stepper.setSpeed(10000);
    stepper.runSpeed();
  }

  delay(1000);

  // Run the motor backwards at 10000 steps/second until the motor reaches
  // position 0 (3/4 revolution):

  while (stepper.currentPosition() != 0) {
    stepper.setSpeed(-10000);
    stepper.runSpeed();
  }

  delay(1000);
}
#endif
