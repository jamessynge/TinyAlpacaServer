#include "extras/host/arduino/random.h"

#include <memory>

#include "absl/random/random.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"

namespace {
std::unique_ptr<absl::BitGen> MakeBitGen() {
  absl::StdSeedSeq time_based_seed({absl::ToUnixMicros(absl::Now())});
  return std::make_unique<absl::BitGen>(time_based_seed);
}

absl::BitGen& GetBitGen() {
  static std::unique_ptr<absl::BitGen> bit_gen(MakeBitGen());  // NOLINT
  return *bit_gen;
}
}  // namespace

int32_t random(int32_t howbig) {
  if (howbig <= 1) {
    return 0;
  }
  return absl::uniform_int_distribution<int32_t>(0, howbig - 1)(GetBitGen());
}

int32_t random(int32_t howsmall, int32_t howbig) {
  if (howsmall >= howbig) {
    return howsmall;
  }
  int32_t diff = howbig - howsmall;
  if (diff <= 1) {
    return howsmall;
  }
  return absl::uniform_int_distribution<int32_t>(howsmall,
                                                 howbig - 1)(GetBitGen());
}
