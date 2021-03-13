#include "extras/host_arduino/arduino.h"

#include "absl/time/clock.h"
#include "absl/time/time.h"

// Style guide says (approximately) don't almost anything (interesting) prior to
// main() executing, but we don't otherwise have a very tidy means of finding
// the time this process started. For more interesting approaches, see
// https://stackoverflow.com/a/2598284.
static const absl::Time start_time = absl::Now();  // NOLINT

uint32_t millis() {
  auto elapsed = absl::Now() - start_time;
  auto elapsed_ms = absl::ToInt64Milliseconds(elapsed);
  return static_cast<uint32_t>(elapsed_ms);
}
