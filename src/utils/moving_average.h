#ifndef TINY_ALPACA_SERVER_SRC_UTILS_MOVING_AVERAGE_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_MOVING_AVERAGE_H_

// MovingAverage incrementally estimates a moving average over some period of
// time from a sequence of discrete readings. Note that there is no implied unit
// for the time and duration variables, though the duration between time 0 and
// time 1 must be 1 (i.e. they have the same step size).
//
// Author: james.synge@gmail.com

#include <McuCore.h>

namespace alpaca {

class MovingAverage {
 public:
  MovingAverage();

  // Updates fields based on a new value to incorporate into the moving average.
  //
  // We assume that new_value represents the average value over the most recent
  // time step, computed as:
  //
  //    time_step = current_time - last_update_time_
  //
  // We further assume the time source can wrap around to zero, but that is rare
  // relative to the number of calls to RecordNewValue, and that only a small
  // amount of time will have passed such that time_step is much smaller than
  // the total time that a uint32_t can represent.
  //
  // average_period is the desired interval over which the values are averaged.
  void RecordNewValue(double new_value, uint32_t current_time,
                      uint32_t average_period);

  uint32_t last_update_time() const { return last_update_time_; }
  double average_value() const { return average_value_; }
  bool has_average_value() const;

 private:
  // The moving average value over the period. Note that we treat this as the
  // moving average over the average_period, even when we only have one reading
  // recorded.
  double average_value_;

  // Time of the most recent call to RecordNewValue.
  uint32_t last_update_time_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_MOVING_AVERAGE_H_
