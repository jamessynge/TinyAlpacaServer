#include "extras/host/arduino/avr_io.h"

volatile uint8_t TCCR1A, TCCR1B, OCR1A, OCR1B, OCR1C;
volatile uint8_t TCCR3A, TCCR3B, OCR3A, OCR3B, OCR3C;
volatile uint8_t TCCR4A, TCCR4B, OCR4A, OCR4B, OCR4C;
volatile uint8_t TCCR5A, TCCR5B, OCR5A, OCR5B, OCR5C;

volatile uint16_t ICR1, ICR3, ICR4, ICR5;
