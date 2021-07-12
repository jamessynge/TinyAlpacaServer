#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_AVR_IO_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_AVR_IO_H_

#include "extras/host/arduino/int_types.h"  // IWYU pragma: export

#ifndef AVR_IO_REGISTER_LINKAGE
#define AVR_IO_REGISTER_LINKAGE extern
#endif  // !AVR_IO_REGISTER_LINKAGE

#define DEFINE_AVR_16_BIT_TIMER_COUNTER_REGISTERS_AND_FIELDS(n)                \
  AVR_IO_REGISTER_LINKAGE volatile uint8_t TCCR##n##A, TCCR##n##B, TCCR##n##C; \
  AVR_IO_REGISTER_LINKAGE volatile uint8_t OCR##n##A, OCR##n##B, OCR##n##C;    \
  AVR_IO_REGISTER_LINKAGE volatile uint8_t TIMSK##n, TIFR##n;                  \
  AVR_IO_REGISTER_LINKAGE volatile uint16_t TCNT##n, OCR##n, ICR##n;           \
  constexpr uint8_t COM##n##A1 = 7;                                            \
  constexpr uint8_t COM##n##A0 = 6;                                            \
  constexpr uint8_t COM##n##B1 = 5;                                            \
  constexpr uint8_t COM##n##B0 = 4;                                            \
  constexpr uint8_t COM##n##C1 = 3;                                            \
  constexpr uint8_t COM##n##C0 = 2;                                            \
  /* Waveform Generation bit fields. */                                        \
  constexpr uint8_t WGM##n##1 = 1;                                             \
  constexpr uint8_t WGM##n##0 = 0;                                             \
  constexpr uint8_t WGM##n##3 = 4;                                             \
  constexpr uint8_t WGM##n##2 = 3;                                             \
  /* Clock select bit fields. */                                               \
  constexpr uint8_t CS##n##2 = 2;                                              \
  constexpr uint8_t CS##n##1 = 1;                                              \
  constexpr uint8_t CS##n##0 = 0;                                              \
  /* Interrupt Mask Register bit fields. */                                    \
  constexpr uint8_t ICIE##n = 5;                                               \
  constexpr uint8_t OCIE##n##C = 3;                                            \
  constexpr uint8_t OCIE##n##B = 2;                                            \
  constexpr uint8_t OCIE##n##A = 1;                                            \
  constexpr uint8_t TOIE##n = 0;                                               \
  /* Interrupt Flag Register bit fields. */                                    \
  constexpr uint8_t ICF##n = 5;                                                \
  constexpr uint8_t OCF##n##C = 3;                                             \
  constexpr uint8_t OCF##n##B = 2;                                             \
  constexpr uint8_t OCF##n##A = 1;                                             \
  constexpr uint8_t TOV##n = 0

DEFINE_AVR_16_BIT_TIMER_COUNTER_REGISTERS_AND_FIELDS(1);
DEFINE_AVR_16_BIT_TIMER_COUNTER_REGISTERS_AND_FIELDS(3);
DEFINE_AVR_16_BIT_TIMER_COUNTER_REGISTERS_AND_FIELDS(4);
DEFINE_AVR_16_BIT_TIMER_COUNTER_REGISTERS_AND_FIELDS(5);

#define DEFINE_AVR_GPIO_PORT_FIELDS(port, register)               \
  constexpr uint8_t register##port##7 = 7, register##port##6 = 6, \
                    register##port##5 = 5, register##port##4 = 4, \
                    register##port##3 = 3, register##port##2 = 2, \
                    register##port##1 = 1, register##port##0 = 0

#define DEFINE_AVR_GPIO_PORT_REGISTERS_AND_FIELDS(port)                      \
  AVR_IO_REGISTER_LINKAGE volatile uint8_t PIN##port, DDR##port, PORT##port; \
  DEFINE_AVR_GPIO_PORT_FIELDS(port, PIN);                                    \
  DEFINE_AVR_GPIO_PORT_FIELDS(port, DD);                                     \
  DEFINE_AVR_GPIO_PORT_FIELDS(port, P)

DEFINE_AVR_GPIO_PORT_REGISTERS_AND_FIELDS(A);
DEFINE_AVR_GPIO_PORT_REGISTERS_AND_FIELDS(B);
DEFINE_AVR_GPIO_PORT_REGISTERS_AND_FIELDS(C);
DEFINE_AVR_GPIO_PORT_REGISTERS_AND_FIELDS(D);
DEFINE_AVR_GPIO_PORT_REGISTERS_AND_FIELDS(E);
DEFINE_AVR_GPIO_PORT_REGISTERS_AND_FIELDS(F);
DEFINE_AVR_GPIO_PORT_REGISTERS_AND_FIELDS(G);

AVR_IO_REGISTER_LINKAGE volatile uint8_t MCUSR;
constexpr uint8_t JTRF = 4;
constexpr uint8_t WDRF = 3;
constexpr uint8_t BORF = 2;
constexpr uint8_t EXTRF = 1;
constexpr uint8_t PORF = 0;

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

#define PIN_WIRE_SDA (20)
#define PIN_WIRE_SCL (21)

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

// From avr-libc's include/avr/sfr_defs.h
#define _BV(bit) (1 << (bit))

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_AVR_IO_H_
