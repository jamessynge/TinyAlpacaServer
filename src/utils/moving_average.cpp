#include "utils/moving_average.h"

#include <math.h>

#include "utils/inline_literal.h"
#include "utils/logging.h"
#include "utils/platform.h"

namespace alpaca {
MovingAverage::MovingAverage()
    : average_value_(INFINITY), last_update_time_(0) {}

bool MovingAverage::has_average_value() const { return !isinf(average_value_); }

void MovingAverage::RecordNewValue(double new_value, uint32_t current_time,
                                   uint32_t average_period) {
  if (has_average_value()) {
    uint32_t time_step = current_time - last_update_time_;
    if (time_step == 0) {
      TAS_VLOG(1) << TASLIT(
          "MovingAverage::RecordNewValue called too quickly, current_time "
          "hasn't advanced beyond last_update_time_");
      return;
    }
    // In order to remove the effect of values before the average_period we must
    // more heavily weight the new value than just (time_step / average_period).
    // This is not a well founded adjustment; TBD later.
    time_step *= 2;
    if (average_period <= time_step) {
      TAS_VLOG(1) << TASLIT("average_period (") << average_period
                  << TASLIT(") should be much greater than time_step (")
                  << time_step << ')';
      average_value_ = new_value;
    } else {
      average_value_ = (average_value_ * (average_period - time_step) +
                        new_value * time_step) /
                       average_period;
    }
  } else {
    // First value.
    average_value_ = new_value;
  }
  last_update_time_ = current_time;
}

}  // namespace alpaca
