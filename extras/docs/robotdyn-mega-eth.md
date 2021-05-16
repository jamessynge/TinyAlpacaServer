## Pin Avoidance

Pins we must, should or may want to avoid on the Robotdyn Mega ETH:

Arduino Mega Pin | Avoid? | Reserved or Optional Purpose
---------------- | ------ | -------------------------------
D0 (RX)          | Should | Serial Input
D1 (TX)          | Should | Serial Output
D4               | Must   | Micro SD Chip Select
D7               | Should | W5500 Hard Reset
D8               | Maybe  | W5500 Interrupt
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
    is received or when a new TCP connection is established). We're not planning
    to need these interrupts, so we may use D8 for other purposes (e.g. for PWM
    output associated with OC4C).

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

## Available GPIO Pins

It is useful to avoid the 2x18 header that has GPIOs 22 through 53 because we
can use that space on the shield for a terminal strip used for stepper motor and
limit switch connectors. This leaves us with the pins below:

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

## Pin Selection

The "unchanged" and "was N" comments are relative to Rev. 6 of the AstroMakers
Cover Calibrator schematic.

Purpose                        | Arduino Mega Pin | Alt. Func. | Comment
------------------------------ | ---------------- | ---------- | -------
LED #1 Pwm Output              | D2               | OC3B       | was D5
LED #2 Pwm Output              | D3               | OC3C       | was D6
LED #3 Pwm Output              | D5               | OC3A       | was D7
LED #4 Pwm Output              | D6               | OC4A       | was D8
LED #2 Enabled Input           | A1               |            | was D10
LED #3 Enabled Input           | A2               |            | was D11
LED #4 Enabled Input           | A3               |            | was D12
Cover Motor Enabled Input      | A4               |            | was D13
Cover Motor Step Output        | D16              |            | was D3
Cover Motor Direction Output   | D17              |            | was D4
Cover Open Limit Switch Input  | D18              | INT1       | was D20
Cover Close Limit Switch Input | D19              | INT0       | was D21

Notes:

*   The exact mapping of LED1 to the proposed PWM pins D2, D3, D5 and D6 isn't
    important, just that those are the PWM pins to use for LEDs.

*   A similar situation applies to the *enabled input* pins A1 to A4: any 4
    Arduino Mega `An` pin will be fine for this purpose.

*   The cover motor step and direction pins D16 and D17 can be swapped with each
    other.

*   The cover open and closed limit pins D18 and D19 can be swapped with each
    other.

## Pin Selection Analysis

TODO: Update to match CoverCalibrator/src/constants.h and the table above, and
trim some of the rambling text.

For the limit switches, we've already selected pins that can be used for
triggering an interrupt when one of those pins is held low AND the External
Interrupt Control Register A (for INT0 through INT3) enables that pin to
interrupt. This means that we should do the following when preparing to close
the cover:

1.  Set volatile field current_action_ that indicates we're moving the cover
    towards closed (e.g. an enum field, where the enum has enumerators
    kNotMoving, kOpening, kClosing).
2.  Enable the "pin low" interrupt for detecting that the cover is closed (e.g.
    INT0 is LOW). When that interrupt is trigged, it should set field
    current_action_ to kNotMoving and then should disable itself; note that it
    is possible that the switch is bouncy, and may first indicate the limit is
    reached slightly earlier (see below); it that turns out to be true, we may
    need to debounce in either the interrupt handler, or in some main loop()
    code.
3.  Configure PWM or a timer/counter with interrupt on overflow or match; the
    latter would have the benefit that the PWM pulse isn't automatically
    generated, but instead allows a small amount of logic to decide whether to
    emit the pulse, including doing debouncing before disabling the timer.

Arduino Mega pins D11 and D12, i.e. PB5 and PB6, can be used as 16-bit
Timer/Counter 1 outputs OC1A and OC1B, can also be used as Pin Change
Interrupts.

Limit switches:

The limit switches are intended to tell the software when it should stop moving
the cover, and at startup they could be used to determine whether the cover
should be closed. I assume that the switches may be bouncy, and may trigger
before the cover has reached the full limit. Furthermore, both switches are
located near the hinge rather than near the far side of the cover, so they're
likely to be rather sensitive to exact placement: if you move them a little
closer to the cover, they'll trigger to soon; if you move them a little farther
away, they may sometimes fail to trigger.

Testing will be needed to determine exactly when the switches trigger and
whether they're well placed.

## Movement Limits

The prototype Cover-Calibrator uses this stepper motor with a 90.25:1 reduction
gear:

https://www.omc-stepperonline.com/nema-8-stepper-motor-bipolar-l-38mm-w-gear-ratio-90-1-planetary-gearbox.html

A full step is 1.8 degress, and the Arduino Shield we're using configures the
stepper driver to do 1/8th microsteps. To rotate 270 degress (i.e. fully open to
fully closed, or visa versa), requires 108300 microsteps:

`270 / 1.8 * 8 * 90.25 = 108300`

Alan's prototype sketch configured AccelStepper to run at 10K steps per second,
or around 10.8 seconds to open and close. Later experiments shows that I could
drive the motor at up to 20K steps per second via interrupts, if I included
acceleration. Beyond that the torque wasn't sufficient to hold it in the closed
position after stopping abruptly. It is possible that with deceleration, maybe
just for a modest number of microsteps after the limit switch triggers, might be
sufficient to allow the motor to hold the cover.

[This file](https://www.omc-stepperonline.com/download/8HS11-0204S_Torque_Curve.pdf)
contains the Pull-Out Torque graph for the stepper motor (without the reduction
gear).

In addition to using the limit switches, it is probably a good idea to stop or
otherwise alert if the number of steps taken is sufficient beyond the expected
number.

The stepper driver chip requires that a pulse be HIGH for at least 1
microsecond, followed by LOW for at least 1 microsecond. If the stepper driver
and stepper could move that fast, we'd be able to move 500K 1/8 microsteps per
second, i.e. a full sweep of the cover would take just over 2/10 of a second,
dangerously fast. However, the torque isn't sufficient for that.
