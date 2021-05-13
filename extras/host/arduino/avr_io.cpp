#include "extras/host/arduino/avr_io.h"

volatile uint8_t TCCR1A, TCCR1B, TCCR1C, OCR1A, OCR1B, OCR1C, TIMSK1;
volatile uint8_t TCCR3A, TCCR3B, TCCR3C, OCR3A, OCR3B, OCR3C, TIMSK3;
volatile uint8_t TCCR4A, TCCR4B, TCCR4C, OCR4A, OCR4B, OCR4C, TIMSK4;
volatile uint8_t TCCR5A, TCCR5B, TCCR5C, OCR5A, OCR5B, OCR5C, TIMSK5;

volatile uint16_t TCNT1, TCNT3, TCNT4, TCNT5;
volatile uint16_t OCR1, OCR3, OCR4, OCR5;
volatile uint16_t ICR1, ICR3, ICR4, ICR5;
