#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_AVR_IO_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_AVR_IO_H_

#include "extras/host/arduino/int_types.h"  // IWYU pragma: export

extern volatile uint8_t TCCR1A, TCCR1B, TCCR1C, OCR1A, OCR1B, OCR1C;
extern volatile uint8_t TCCR3A, TCCR3B, TCCR3C, OCR3A, OCR3B, OCR3C;
extern volatile uint8_t TCCR4A, TCCR4B, TCCR4C, OCR4A, OCR4B, OCR4C;
extern volatile uint8_t TCCR5A, TCCR5B, TCCR5C, OCR5A, OCR5B, OCR5C;

extern volatile uint16_t ICR1, ICR3, ICR4, ICR5;

constexpr uint8_t COM1A1 = 7;
constexpr uint8_t COM1A0 = 6;
constexpr uint8_t COM1B1 = 5;
constexpr uint8_t COM1B0 = 4;
constexpr uint8_t COM1C1 = 3;
constexpr uint8_t COM1C0 = 2;
constexpr uint8_t WGM11 = 1;
constexpr uint8_t WGM10 = 0;

constexpr uint8_t WGM13 = 4;
constexpr uint8_t WGM12 = 3;
constexpr uint8_t CS12 = 2;
constexpr uint8_t CS11 = 1;
constexpr uint8_t CS10 = 0;

constexpr uint8_t COM3A1 = 7;
constexpr uint8_t COM3A0 = 6;
constexpr uint8_t COM3B1 = 5;
constexpr uint8_t COM3B0 = 4;
constexpr uint8_t COM3C1 = 3;
constexpr uint8_t COM3C0 = 2;
constexpr uint8_t WGM31 = 1;
constexpr uint8_t WGM30 = 0;

constexpr uint8_t WGM33 = 4;
constexpr uint8_t WGM32 = 3;
constexpr uint8_t CS32 = 2;
constexpr uint8_t CS31 = 1;
constexpr uint8_t CS30 = 0;

constexpr uint8_t COM4A1 = 7;
constexpr uint8_t COM4A0 = 6;
constexpr uint8_t COM4B1 = 5;
constexpr uint8_t COM4B0 = 4;
constexpr uint8_t COM4C1 = 3;
constexpr uint8_t COM4C0 = 2;
constexpr uint8_t WGM41 = 1;
constexpr uint8_t WGM40 = 0;

constexpr uint8_t WGM43 = 4;
constexpr uint8_t WGM42 = 3;
constexpr uint8_t CS42 = 2;
constexpr uint8_t CS41 = 1;
constexpr uint8_t CS40 = 0;

constexpr uint8_t COM5A1 = 7;
constexpr uint8_t COM5A0 = 6;
constexpr uint8_t COM5B1 = 5;
constexpr uint8_t COM5B0 = 4;
constexpr uint8_t COM5C1 = 3;
constexpr uint8_t COM5C0 = 2;
constexpr uint8_t WGM51 = 1;
constexpr uint8_t WGM50 = 0;

constexpr uint8_t WGM53 = 4;
constexpr uint8_t WGM52 = 3;
constexpr uint8_t CS52 = 2;
constexpr uint8_t CS51 = 1;
constexpr uint8_t CS50 = 0;

#define PIN_A0 (54)
#define PIN_A1 (55)
#define PIN_A2 (56)
#define PIN_A3 (57)
#define PIN_A4 (58)
#define PIN_A5 (59)
#define PIN_A6 (60)
#define PIN_A7 (61)
#define PIN_A8 (62)
#define PIN_A9 (63)
#define PIN_A10 (64)
#define PIN_A11 (65)
#define PIN_A12 (66)
#define PIN_A13 (67)
#define PIN_A14 (68)
#define PIN_A15 (69)

#define NOT_AN_INTERRUPT 0xff

#define digitalPinToInterrupt(p) \
  ((p) == 2                      \
       ? 0                       \
       : ((p) == 3 ? 1           \
                   : ((p) >= 18 && (p) <= 21 ? 23 - (p) : NOT_AN_INTERRUPT)))

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_AVR_IO_H_
