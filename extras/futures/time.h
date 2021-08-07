#ifndef TINY_ALPACA_SERVER_EXTRAS_FUTURES_TIME_H_
#define TINY_ALPACA_SERVER_EXTRAS_FUTURES_TIME_H_

// Time and Duration classes to bring time computations together into one
// location.
//
// Author: james.synge@gmail.com

#include "mcucore_platform.h"

namespace alpaca {

class Duration;
class Time;

namespace time_internal {
// Functions for internal use by functions in this file.
uint32_t repr(Time t);
int32_t repr(Duration d);
Time repr_to_time(uint32_t ms);
Duration repr_to_duration(int32_t ms);
}  // namespace time_internal

// This is used for Time and Duration. The field `negative` is false
// when produced from a Time.
struct TimeParts : Printable {
  explicit TimeParts(uint32_t ms);
  size_t printTo(Print&) const override;
  uint16_t milliseconds;
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint8_t days;
  bool negative;  // false == positive; true == negative;
};

Duration operator-(Time a, Time b);
bool operator>=(Time a, Time b);
Time operator-(Time t, Duration d);
Time operator+(Time t, Duration d);
Duration operator-(Duration a, Duration b);
Duration operator+(Duration a, Duration b);
Duration operator/(Duration dur, int32_t div);
Duration operator*(Duration dur, int32_t mul);
Duration operator/(Duration dur, double div);
Duration operator*(Duration dur, double mul);
bool operator>=(Duration a, Duration b);
bool operator>(Duration a, Duration b);
bool operator<=(Duration a, Duration b);
bool operator<(Duration a, Duration b);
Duration Milliseconds(int ms);
Duration Milliseconds(int32_t ms);
Duration Milliseconds(double ms);
Duration Seconds(int seconds);
Duration Seconds(int32_t seconds);
Duration Seconds(double seconds);
Duration Minutes(int minutes);
Duration Minutes(int32_t minutes);
Duration Minutes(double minutes);
Duration Hours(int hours);
Duration Hours(int32_t hours);
Duration Hours(double hours);

class Time : public Printable {
 public:
  // Start of epoch, which for an Arduino is when it booted or rolled over.
  Time();
  Time(const Time&) = default;

  Time& operator=(Time);
  Time& operator+=(Duration);
  TimeParts Split() const;
  size_t printTo(Print&) const override;

  static Time Now();

  friend bool operator==(Time a, Time b);

 private:
  friend uint32_t time_internal::repr(Time t);
  friend Time time_internal::repr_to_time(uint32_t ms);

  explicit Time(uint32_t ms);
  uint32_t ms_;
};

class Duration : public Printable {
 public:
  Duration() : ms_(0) {}
  Duration(const Duration&) = default;
  Duration& operator=(const Duration&) = default;
  Duration& operator+=(const Duration&);

  TimeParts Split() const;
  size_t printTo(Print&) const override;

 private:
  friend int32_t time_internal::repr(Duration d);
  friend Duration time_internal::repr_to_duration(int32_t ms);

  explicit Duration(int32_t ms);
  int32_t ms_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_FUTURES_TIME_H_
