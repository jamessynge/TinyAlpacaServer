#include "utils/moving_average.h"

#include <math.h>

#include "inline_literal.h"
#include "logging.h"
#include "mcucore_platform.h"
#include "progmem_string_data.h"

namespace alpaca {
MovingAverage::MovingAverage()
    : average_value_(INFINITY), last_update_time_(0) {}

bool MovingAverage::has_average_value() const { return !isinf(average_value_); }

void MovingAverage::RecordNewValue(double new_value, uint32_t current_time,
                                   uint32_t average_period) {
  if (has_average_value()) {
    uint32_t time_step = current_time - last_update_time_;
    if (time_step == 0) {
      MCU_VLOG(1) << MCU_FLASHSTR_128(
          "MovingAverage::RecordNewValue called too quickly, current_time "
          "hasn't advanced beyond last_update_time_");
      return;
    }
    // In order to remove the effect of values before the average_period we must
    // more heavily weight the new value than just (time_step / average_period).
    // This is not a well founded adjustment; TBD later.
    time_step *= 2;
    if (average_period <= time_step) {
      MCU_VLOG(1) << MCU_FLASHSTR("average_period (") << average_period
                  << MCU_FLASHSTR(") should be much greater than time_step (")
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
