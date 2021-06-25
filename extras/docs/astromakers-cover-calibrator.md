# AAVSO AstroMakers Cover Calibrator

This doc provides info about the AAVSO AstroMakers Cover Calibrator designed by
Alan Sliski, with ASCOM Alpaca server software designed and implemented by James
Synge. In particular it provides info about the pin mapping for the Robotdyn
Mega ETH, and extension features of the software.

## Selecting the Enabled LED Channels

The cover calibrator has 4 LED channels, but the ASCOM API has only a single
channel, therefore we need a means of selecting which LED channels are enabled.

## Pin Recommendations for Shield Revision 7

An analysis of the pins of the Robotdyn Mega ETH and their availability for use
is in the document
[Robotdyn Mega ETH Pin Availability Analysis](./robotdyn-mega-eth.md). Based on
that, the following table has the recommended pins to be used for the cover
calibrator. The "unchanged" and "was N" comments are relative to Rev. 6 of the
AstroMakers Cover Calibrator schematic.

Purpose                         | Arduino Mega Pin | Alt. Func.     | Comment
------------------------------- | ---------------- | -------------- | -------
LED #1 Pwm Output               | D2               | OC3B           | was D5
LED #2 Pwm Output               | D3               | OC3C           | was D6
LED #3 Pwm Output               | D5               | OC3A           | was D7
LED #4 Pwm Output               | D6               | OC4A           | was D8
LED #1 Enabled Input            | A0               | ADC0           | **new**
LED #2 Enabled Input            | A1               | ADC1           | was D10
LED #3 Enabled Input            | A2               | ADC2           | was D11
LED #4 Enabled Input            | A3               | ACD3           | was D12
Cover Motor Step Output         | D16              | TXD2           | was D3
Cover Motor Direction Output    | D17              | RXD2           | was D4
Cover Open Limit Switch Input   | D18              | INT1           | was D20
Cover Close Limit Switch Input  | D19              | INT0           | was D21
Cover Motor Enabled Input       | A8               | ADC8, PCINT16  | was D13
Microstep Resolution MS1 Output | A9               | ADC9, PCINT17  | **new**
Microstep Resolution MS2 Output | A10              | ADC10, PCINT18 | **new**
Microstep Resolution MS3 Output | A11              | ADC11, PCINT19 | **new**
Cover Motor Enabled Input       | A12              | ADC12, PCINT20 | **new**
LED PWM Resolution Input        | A13              | ADC13, PCINT21 | **new**

With the exception of the pin purposes marked as new, these recommendations have
been tested by using a Rev. 6 shield and a set of jumper cables mapping its pins
to the proposed pins.

Notes:

*   The exact mapping of LED PWM output purposes to the proposed PWM pins D2,
    D3, D5 and D6 isn't important (i.e. the order can change). If the order does
    change, then the code in examplesCoverCalibrator/src/cover_calibrator.* will
    need to be updated because I've not written code that handles figuring out
    the Timer/Counter registers based on a pin number.

*   A similar situation applies to the 5 *enabled input* pins and the 3
    *Microstepping Resolution* pins: the exact mapping from purpose to those 7
    pins isn't important. There are also another 4 analog pins (A12-A15) that
    can be selected if those are easier to layout traces for.

*   The cover motor step and direction pins D16 and D17 can be swapped with each
    other.

*   The cover open and closed limit pins D18 and D19 can be swapped with each
    other. It happens that the current software (as of June 11, 2021) is not
    taking advantage of the fact that the proposed limit switch pins can be used
    to trigger an interrupt, but that may be useful later.

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
