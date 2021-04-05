#include "extras/futures/time.h"

namespace alpaca {

namespace time_internal {

// Functions for internal use by functions in this file.
uint32_t repr(Time t) { return t.ms_; }
int32_t repr(Duration d) { return d.ms_; }

Time repr_to_time(uint32_t ms) { return Time(ms); }
Duration repr_to_duration(int32_t ms) { return Duration(ms); }

}  // namespace time_internal

using time_internal::repr;
using time_internal::repr_to_duration;
using time_internal::repr_to_time;

namespace {
size_t printWithLeadingZeros(Print& p, uint32_t value, int min_width) {
  size_t result = 0;
  if (value >= 10) {
    result += printWithLeadingZeros(p, value / 10, min_width - 1);
    value = value % 10;
    min_width = 0;
  }
  while (min_width > 1) {
    result += p.print('0');
    --min_width;
  }
  return result + p.print(value, DEC);
}
}  // namespace

TimeParts::TimeParts(uint32_t ms) {
  negative = false;
  milliseconds = ms % 1000;
  ms /= 1000;
  seconds = ms % 60;
  ms /= 60;
  minutes = ms % 60;
  ms /= 60;
  hours = ms % 24;
  ms /= 24;
  days = ms;
}

size_t TimeParts::printTo(Print& p) const {
  size_t result = 0;
  if (negative) {
    result += p.print('-');
  }
  bool first = true;
  if (days > 0) {
    first = false;
    result += p.print(days, DEC);
    result += p.print("d ");
  }
  if (!first || hours > 0) {
    first = false;
    result += printWithLeadingZeros(p, hours, 2);
    result += p.print(":");
  }
  result += printWithLeadingZeros(p, minutes, 2);
  result += p.print(":");
  result += printWithLeadingZeros(p, seconds, 2);
  result += p.print(".");
  result += printWithLeadingZeros(p, milliseconds, 3);
  return result;
}

Time::Time() : ms_(0) {}
Time::Time(uint32_t ms) : ms_(ms) {}
Time& Time::operator=(Time other) {
  ms_ = other.ms_;
  return *this;
}
Time& Time::operator+=(Duration d) {
  ms_ += repr(d);
  return *this;
}
TimeParts Time::Split() const { return TimeParts(ms_); }
// static
Time Time::Now() { return Time(millis()); }
size_t Time::printTo(Print& p) const { return Split().printTo(p); }

Duration::Duration(int32_t ms) : ms_(ms) {}

TimeParts Duration::Split() const {
  if (ms_ >= 0) {
    return TimeParts(ms_);
  }
  TimeParts p(-ms_);
  p.negative = true;
  return p;
}

size_t Duration::printTo(Print& p) const { return Split().printTo(p); }

Duration operator-(Time a, Time b) {
  auto a_ms = repr(a);
  auto b_ms = repr(b);
  if (a_ms >= b_ms) {
    return repr_to_duration(a_ms - b_ms);
  } else {
    return repr_to_duration(-(b_ms - a_ms));
  }
}
bool operator>=(Time a, Time b) { return repr(a) >= repr(b); }
Time operator-(Time t, Duration d) {
  auto t_ms = repr(t);
  auto d_ms = repr(d);
  return repr_to_time(t_ms - d_ms);
}
Time operator+(Time t, Duration d) {
  auto t_ms = repr(t);
  auto d_ms = repr(d);
  return repr_to_time(t_ms + d_ms);
}
Duration operator-(Duration a, Duration b) {
  auto a_ms = repr(a);
  auto b_ms = repr(b);
  return repr_to_duration(a_ms - b_ms);
}
Duration operator+(Duration a, Duration b) {
  auto a_ms = repr(a);
  auto b_ms = repr(b);
  return repr_to_duration(a_ms + b_ms);
}
Duration operator/(Duration dur, int32_t div) {
  auto dur_ms = repr(dur);
  return repr_to_duration(dur_ms / div);
}
Duration operator*(Duration dur, int32_t mul) {
  auto dur_ms = repr(dur);
  return repr_to_duration(dur_ms * mul);
}
Duration operator/(Duration dur, double div) {
  auto dur_ms = repr(dur);
  return repr_to_duration(dur_ms / div);
}
Duration operator*(Duration dur, double mul) {
  auto dur_ms = repr(dur);
  return repr_to_duration(dur_ms * mul);
}
bool operator>=(Duration a, Duration b) {
  auto a_ms = repr(a);
  auto b_ms = repr(b);
  return a_ms >= b_ms;
}
bool operator>(Duration a, Duration b) {
  auto a_ms = repr(a);
  auto b_ms = repr(b);
  return a_ms > b_ms;
}
bool operator<=(Duration a, Duration b) {
  auto a_ms = repr(a);
  auto b_ms = repr(b);
  return a_ms <= b_ms;
}
bool operator<(Duration a, Duration b) {
  auto a_ms = repr(a);
  auto b_ms = repr(b);
  return a_ms < b_ms;
}

Duration Milliseconds(int32_t ms) { return repr_to_duration(ms); }
Duration Milliseconds(double ms) {
  return repr_to_duration(static_cast<int32_t>(round(ms)));
}
Duration Seconds(int32_t seconds) { return Milliseconds(seconds * 1000); }
Duration Seconds(double seconds) { return Milliseconds(seconds * 1000); }
Duration Minutes(int32_t minutes) { return Seconds(minutes * 60); }
Duration Minutes(double minutes) { return Seconds(minutes * 60); }
Duration Hours(int32_t hours) { return Minutes(hours * 60); }
Duration Hours(double hours) { return Minutes(hours * 60); }

}  // namespace alpaca
