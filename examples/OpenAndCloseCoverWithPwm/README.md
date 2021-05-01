** Pin Avoidance

Pins we should (or must) avoid on the Robotdyn Mega ETH:

D00 - RX (Serial over USB) D01 - TX (Serial over USB) D04 - SDcard Chip Select
D10 - W5500 Chip Select D13 - Built-in LED D50 - MISO (SPI) D51 - MOSI (SPI)
D52 - SCK (SPI)

** Pin Selection

Purpose                        | Arduino Mega Pin | ATmega2560 Name | Comment
------------------------------ | ---------------- | --------------- | ---------
LED #1 Pwm Output              | 5                | OC3A            | unchanged
LED #2 Pwm Output              | 6                | OC4A            | unchanged
LED #2 Enabled Output          | PIN_A1           |                 | was 10
LED #3 Pwm Output              | 7                | OC4B            | unchanged
LED #3 Enabled Output          | PIN_A2           |                 | was 11
LED #4 Pwm Output              | 8                | OC4C unchanged  |
LED #4 Enabled Output          | PIN_A3           |                 | was 12
Cover Motor Step Output        | 3                |                 | unchanged
Cover Motor Direction Output   | 5                |                 | unchanged
Cover Open Limit Switch Input  | 20               | INT1 unchanged  |
Cover Close Limit Switch Input | 21               | INT0 unchanged  |
Cover Motor Enabled Input      | PIN_A4           |                 | was 13

** Pin Selection Analysis

For the limit switches, we've already selected pins that can be used for
triggering an interrupt when one of those pins is held low AND the External
Interrupt Control Register A (for INT0 through INT3) enables that pin to
interrupt. This means that we should do the following when preparing to close
the cover:

1) Set volatile field current_action_ that indicates we're moving the cover
towards closed (e.g. an enum field, where the enum has enumerators kNotMoving,
kOpening, kClosing). 2) Enable the "pin low" interrupt for detecting that the
cover is closed (e.g. INT0 is LOW). When that interrupt is trigged, it should
set field current_action_ to kNotMoving and then should disable itself; note
that it is possible that the switch is bouncy, and may first indicate the limit
is reached slightly earlier (see below); it that turns out to be true, we may
need to debounce in either the interrupt handler, or in some main loop() code.
3) Configure PWM or a timer/counter with interrupt on overflow or match; the
latter would have the benefit that the PWM pulse isn't automatically generated,
but instead allows a small amount of logic to decide whether to emit the pulse,
including doing debouncing before disabling the timer.

Arduino Mega pins 11 and 12, i.e. PB5 and PB6, can be used as 16-bit T/C 1
outputs OC1A and OC1B, can also be used as Pin Change Interrupts.

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

Movement limits:

The prototype Cover-Calibrator uses this stepper motor with a 90.25:1 reduction
gear:

https://www.omc-stepperonline.com/nema-8-stepper-motor-bipolar-l-38mm-w-gear-ratio-90-1-planetary-gearbox.html

A full step is 1.8 degress, and the Arduino Shield we're using configures the
stepper driver to do 1/8th microsteps. To rotate 270 degress (i.e. fully open to
fully closed, or visa versa), requires 108300 microsteps:

270 / 1.8 * 8 * 90.25 = 108300

In addition to using the limit switches, it is probably a good idea to stop or
otherwise alert if the number of steps taken is sufficient beyond the expected
number.

The stepper driver chip requires that a pulse be HIGH for at least 1
microsecond, followed by LOW for at least 1 microsecond. If the stepper driver
and stepper could move that fast, we'd be able to move 500K 1/8 microsteps per
second, i.e. a full sweep of the cover would take just over 2/10 of a second,
dangerously fast.

Alan's prototype sketch configured AccelStepper to run at 20K steps per second,
or around 5.25 seconds to open and close; the sketch did not use the
acceleration and deceleration features of AccelStepper, so it isn't clear to me
how fast we can actually drive the motor and cover.
