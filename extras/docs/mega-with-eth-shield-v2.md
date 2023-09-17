# Arduino Mega With Ethernet Shield V2 Pin Availability Analysis

## Pin Avoidance

Pins we must, should or may want to avoid on the Arduino Mega:

Arduino Mega Pin | Avoid? | Reserved or Optional Purpose
---------------- | ------ | -------------------------------
D0 (RX)          | Should | Serial Input
D1 (TX)          | Should | Serial Output
D4               | Must   | Micro SD Chip Select
D9               | Should | Micro SD card detect
D10              | Must   | W5500 Chip Select
D13              | Maybe  | Built-in LED
D20 (SDA)        | Maybe  | Two-Wire Serial Interface
D21 (SCL)        | Maybe  | Two-Wire Serial Interface
D50 (MISO)       | Must   | SPI for W5500 and micro SD card
D51 (MOSI/DI)    | Must   | SPI for W5500 and micro SD card
D52 (SCK/CLK)    | Must   | SPI for W5500 and micro SD card
A4 (ADC4/TCK)    | Maybe  | JTAG Test Clock
A5 (ADC5/TMS)    | Maybe  | JTAG Test Mode Select
A6 (ADC6/TDO)    | Maybe  | JTAG Test Data Output
A7 (ADC7/TDI)    | Maybe  | JTAG Test Data Input

Pins we must, should or may want to avoid on the Arduino Ethernet Shield V2
([schematic](https://docs.arduino.cc/static/1400433f59d85c08ee5100d183309882/schematics.pdf)):

Arduino Mega Pin | Avoid? | Reserved or Optional Purpose
---------------- | ------ | -------------------------------
D0 (RX)          | Should | Serial Input
D1 (TX)          | Should | Serial Output
D4               | Must   | Micro SD Chip Select
D9               | Should | Micro SD card detect
D10              | Must   | W5500 Chip Select
D13              | Maybe  | Built-in LED
D14 (TX3)        | Maybe  | Optional ESP-01 Daughter board
D15 (RX3)        | Maybe  | Optional ESP-01 Daughter board
D20 (SDA)        | Maybe  | Two-Wire Serial Interface
D21 (SCL)        | Maybe  | Two-Wire Serial Interface
D50 (MISO)       | Must   | SPI for W5500 and micro SD card
D51 (MOSI/DI)    | Must   | SPI for W5500 and micro SD card
D52 (SCK/CLK)    | Must   | SPI for W5500 and micro SD card
A4 (ADC4/TCK)    | Maybe  | JTAG Test Clock
A5 (ADC5/TMS)    | Maybe  | JTAG Test Mode Select
A6 (ADC6/TDO)    | Maybe  | JTAG Test Data Output
A7 (ADC7/TDI)    | Maybe  | JTAG Test Data Input

New Notes:

RESET on the W5500 is connected to the Arduino RESET pin, but that means that we
can't reset the W5500 by itself, which has proved valuable (essential?). We
might want to NOT connect the shield's RESET pin to the Arduino, and instead
connect it to a pin that we control.

Notes:

*   D0 and D1 are used for Serial input and output. While this feature isn't
    essential in production, it is for debugging, and we might choose to use
    them during configuration of a new device, e.g. running a (separate?) sketch
    to store a MAC address and other hardware information.

*   Chip select data lines D4 and D10 are used together with the 3 required
    [SPI data lines](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface)
    (MISO, MOSI, SCLK, i.e. D50, D51 and D52) to indicate the one peripheral
    with which the host (the ATmega2560 in this case) is communicating. Since we
    require the W5500 for communication and want to use the SD card as a source
    of web pages (and possibly as a place to store configuration data), we need
    to avoid using their chip select pins for other purposes.

*   D7 may be connected to the RSTn pin of W5500 chip, enabling the ATmega2560
    to force a hard reset of the network chip. The connection is formed by
    creating a solder bridge on the back of the board between two adjacent pads
    labeled D7. So far using this feature hasn't been necessary (i.e. soft reset
    has worked at startup), but it seems prudent to leave it available.

*   D8 may be connected to the INTn pin of W5500 chip, enabling it to signal to
    the ATmega2560 for certain conditions occur (e.g. when a Wake-on-LAN packet
    is received or when a new TCP connection is established). We're not
    currently planning to use the interrupt feature of the W5500, so we *may*
    use D8 for other purposes (e.g. for PWM output associated with OC4C), though
    it is probably best to avoid it so as to keep our options open, unless we're
    short on pins

*   D9 may be connected to a Card Detect pin of the Micro SD Card socket on the
    Mega ETH board. This is done by creating a solder bridge on the back of the
    board between two adjacent pads labeled D9.

    NOTE: I'm not certain yet whether D9 is pulled low when a chip is inserted,
    but that seems most likely.

*   D13 is a rather overloaded pin. It can be used as a regular GPIO pin, but is
    also hooked up to an LED on the board (for output), is the output pin for
    two different timer/counter peripherals of the ATmega2560 and can be used as
    a pin-change interrupt input. It is most likely that we'll want to use it as
    an output pin.

*   D14 and D15 may be used for talking to an (optional) ESP-01 daughter board.
    Since we're planning on receiving Power-over-Ethernet and communicating
    using the W5500 (i.e. the reasons that we chose the Robotdyn Mega ETH), it
    is unlikely that also we'll add an ESP-01, so it seems safe to use those two
    pins.

*   D20 and D21 support the two-wire serial interface, which is used by various
    sensors and other peripherals. This is not initially required for the
    AstroMakers cover-calibrator, nor for the weather sensors. However, we may
    come up with a sensor we want to add that requires this interface, so
    keeping these two pins available is a good idea if it isn't too
    inconvenient.

*   Analog pins A4 to A7 are also used for JTAG (testing interface). We don't
    have to avoid these, but it would be nice to keep the option open unless we
    run out of pins.

## Available GPIO Pins

In the case of the Cover Calibrator shield, it is useful to avoid the 2x18
header that has GPIOs 22 through 53 because we can use that space on the shield
for a terminal strip used for stepper motor and limit switch connectors. Also,
if designing a shield for the Arduino MEGA form factor, having too many pins on
the shield can make it difficult to apply even pressure when inserting the pins
of the shield into the MEGA, and exceedingly difficult to remove the shield
without bending the pins.

This leaves us with the pins below:

Arduino Mega Pin | Alternate Function(s)
---------------- | ---------------------
D2               | OC3B, INT4
D3               | OC3C, INT5
D5               | OC3A, AIN1
D6               | OC4A, PCINT8
D11              | OC1A, PCINT5
D12              | OC1B, PCINT6
D14              | TXD3, PCINT10
D15              | RXD3, PCINT9
D16              | TXD2
D17              | RXD2
D18              | TXD1, INT3
D19              | RXD1, INT2
A0               | ADC0
A1               | ADC1
A2               | ADC2
A3               | ADC3
A4               | ADC4, TCK
A5               | ADC5, TMS
A6               | ADC6, TDO
A7               | ADC7, TDI
A8               | ADC8, PCINT16
A9               | ADC9, PCINT17
A10              | ADC10, PCINT18
A11              | ADC11, PCINT19
A12              | ADC12, PCINT20
A13              | ADC13, PCINT21
A14              | ADC14, PCINT22
A15              | ADC15, PCINT23
