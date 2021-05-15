#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_AVR_IO_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_AVR_IO_H_

#include "extras/host/arduino/int_types.h"  // IWYU pragma: export

#ifndef AVR_TIMER_COUNTER_N_REGISTER_LINKAGE
#define AVR_TIMER_COUNTER_N_REGISTER_LINKAGE extern
#endif  // !AVR_TIMER_COUNTER_N_REGISTER_LINKAGE

#define DEFINE_AVR_16_BIT_TIMER_COUNTER_REGISTERS_AND_FIELDS(n)               \
  AVR_TIMER_COUNTER_N_REGISTER_LINKAGE volatile uint8_t TCCR##n##A,           \
      TCCR##n##B, TCCR##n##C;                                                 \
  AVR_TIMER_COUNTER_N_REGISTER_LINKAGE volatile uint8_t OCR##n##A, OCR##n##B, \
      OCR##n##C;                                                              \
  AVR_TIMER_COUNTER_N_REGISTER_LINKAGE volatile uint8_t TIMSK##n, TIFR##n;    \
  AVR_TIMER_COUNTER_N_REGISTER_LINKAGE volatile uint16_t TCNT##n, OCR##n,     \
      ICR##n;                                                                 \
  constexpr uint8_t COM##n##A1 = 7;                                           \
  constexpr uint8_t COM##n##A0 = 6;                                           \
  constexpr uint8_t COM##n##B1 = 5;                                           \
  constexpr uint8_t COM##n##B0 = 4;                                           \
  constexpr uint8_t COM##n##C1 = 3;                                           \
  constexpr uint8_t COM##n##C0 = 2;                                           \
  /* Waveform Generation bit fields. */                                       \
  constexpr uint8_t WGM##n##1 = 1;                                            \
  constexpr uint8_t WGM##n##0 = 0;                                            \
  constexpr uint8_t WGM##n##3 = 4;                                            \
  constexpr uint8_t WGM##n##2 = 3;                                            \
  /* Clock select bit fields. */                                              \
  constexpr uint8_t CS##n##2 = 2;                                             \
  constexpr uint8_t CS##n##1 = 1;                                             \
  constexpr uint8_t CS##n##0 = 0;                                             \
  /* Interrupt Mask Register bit fields. */                                   \
  constexpr uint8_t ICIE##n = 5;                                              \
  constexpr uint8_t OCIE##n##C = 3;                                           \
  constexpr uint8_t OCIE##n##B = 2;                                           \
  constexpr uint8_t OCIE##n##A = 1;                                           \
  constexpr uint8_t TOIE##n = 0;                                              \
  /* Interrupt Flag Register bit fields. */                                   \
  constexpr uint8_t ICF##n = 5;                                               \
  constexpr uint8_t OCF##n##C = 3;                                            \
  constexpr uint8_t OCF##n##B = 2;                                            \
  constexpr uint8_t OCF##n##A = 1;                                            \
  constexpr uint8_t TOV##n = 0

DEFINE_AVR_16_BIT_TIMER_COUNTER_REGISTERS_AND_FIELDS(1);
DEFINE_AVR_16_BIT_TIMER_COUNTER_REGISTERS_AND_FIELDS(3);
DEFINE_AVR_16_BIT_TIMER_COUNTER_REGISTERS_AND_FIELDS(4);
DEFINE_AVR_16_BIT_TIMER_COUNTER_REGISTERS_AND_FIELDS(5);

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

#define digitalPinToPort(p) p

#define portOutputRegister(p) &TCCR1A  // Totally BOGUS

#define digitalPinToBitMask(p) 1

#define ISR(name) void ISR_##name()

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_AVR_IO_H_
