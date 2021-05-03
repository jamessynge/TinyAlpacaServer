** Pin Avoidance

Pins we must, should or may want to avoid on the Robotdyn Mega ETH:

| Arduino Mega Pin | Avoid? | Reserved Purpose                                 |
| ---------------- | ------ | ------------------------------------------------ |
| D0 (RX)          | Must   | Serial Input                                     |
| D1 (TX)          | Must   | Serial Output                                    |
| D4               | Should | Micro SD Chip Select (Must avoid to use SD card) |
| D7               | Maybe  | W5500 Hard Reset (Not connected by default, but  |
:                  :        : Alan has done so on his boards)                  :
| D8               | Maybe  | W5500 Interrupts (Not connected by default)      |
| D9               | Maybe  | Micro SD card detect (Not connected by default,  |
:                  :        : but probably a good idea unless it is easy to to :
:                  :        : so by other means)                               :
| D10              | Must   | W5500 Chip Select (with SPI)                     |
| D13              | Maybe  | Built-in LED (useful for status output, but it   |
:                  :        : is also an ouptut pin for two different          :
:                  :        : timer/counters, 0 and 1)                         :
| D14 (TX3)        | Maybe  | ESP-01 Daughter board (Optional accessory, not   |
:                  :        : used for the AstroMakers CoverCalibrator)        :
| D15 (RX3)        | Maybe  | ESP-01 Daughter board (Optional)                 |
| D50 (MISO)       | Must   | SPI for W5500 and micro SD card                  |
| D51 (MOSI/DI)    | Must   | SPI for W5500 and micro SD card                  |
| D52 (SCK/CLK)    | Must   | SPI for W5500 and micro SD card                  |

** Pin Selection

The "unchanged" and "was N" comments are relative to Rev. 6 of the AstroMakers
Cover Calibrator schematic.

Purpose                        | Arduino Mega Pin | ATmega2560 Name | Comment
------------------------------ | ---------------- | --------------- | ---------
LED #1 Pwm Output              | 5                | OC3A            | unchanged
LED #2 Pwm Output              | 6                | OC4A            | unchanged
LED #2 Enabled Output          | A1               |                 | was 10
LED #3 Pwm Output              | 7                | OC4B            | unchanged
LED #3 Enabled Output          | A2               |                 | was 11
LED #4 Pwm Output              | 8                | OC4C unchanged  |
LED #4 Enabled Output          | A3               |                 | was 12
Cover Motor Step Output        | 3                |                 | unchanged
Cover Motor Direction Output   | 5                |                 | unchanged
Cover Open Limit Switch Input  | 20               | INT1 unchanged  |
Cover Close Limit Switch Input | 21               | INT0 unchanged  |
Cover Motor Enabled Input      | A4               |                 | was 13

Arduino Mega Pin | ATmega2560 Name | Purpose                 | Comment
---------------- | --------------- | ----------------------- | -------------
0                | PCINT8          | reserved                | Serial Input
1                | PCINT3          | reserved                | Serial Output
2                | INT4 / OC3B     | unused                  |
3                |                 | Cover Motor Step Output | zzz
4                | xxx             | xxx                     | zzz
5                | xxx             | xxx                     | zzz
6                | xxx             | xxx                     | zzz
7                | xxx             | xxx                     | zzz
8                | xxx             | xxx                     | zzz
9                | xxx             | xxx                     | zzz
10               | xxx             | xxx                     | zzz
11               | xxx             | xxx                     | zzz
12               | xxx             | xxx                     | zzz
13               | xxx             | xxx                     | zzz

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
