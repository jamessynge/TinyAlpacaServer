// Provides the minimal wrapper around setup() and loop()

#include "absl/flags/flag.h"
#include "base/init_google.h"

ABSL_FLAG(int, num_loops, 1, "Number of times to execute loop");

void setup();
void loop();

int main(int argc, char* argv[]) {
  InitGoogle(argv[0], &argc, &argv, /*remove_flags=*/true);
  setup();
  for (int i = 0; i < absl::GetFlag(FLAGS_num_loops); ++i) {
    loop();
  }
  return 0;
}
