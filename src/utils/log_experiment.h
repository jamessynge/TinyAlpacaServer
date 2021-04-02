#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LOG_EXPERIMENT_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LOG_EXPERIMENT_H_

// // This is really a test of how well the compiler and linker can handle NOT
// // emitting reachable code into the final binary. This file is included from
// // sketches TestLogExperimentDisabled and TestLogExperimentEnabled.

// #ifdef TAS_ENABLE_LOG_EXPERIMENT

// #include "utils/logging.h"
// #include "utils/platform.h"

// inline void LogExperimentTestFunction() {
// #if TAS_HOST_TARGET
//   LOG(INFO) << "LogExperimentTestFunction enter";
// #endif

//   TAS_VLOG(1) << "LogExperimentTestFunction enter";

//   if (false) {
//   } else  // NOLINT: Checking the else behavior.
//     TAS_VLOG(1) << "An integer " << 1 << ", followed by a float " << 3.14;

//   if (true)
//     TAS_VLOG(1) << "An integer " << 1 << ", followed by a float " << 3.14;
//   else {  // NOLINT: Checking the if behavior.
//     TAS_VLOG(1) << "Should NOT be executed";
//   }

//   if (true)
//     TAS_CHECK(true) << "An integer " << 1 << ", followed by a float " << 3.14;
//   else
//     TAS_VLOG(1) << "SHOULD be executed";

//   if (false) {
//   } else  // NOLINT: Checking the else behavior.
//     TAS_CHECK(false) << "An integer " << 1 << ", followed by a float " << 3.14;

//   TAS_VLOG(1) << "LogExperimentTestFunction exit";

// #if TAS_HOST_TARGET
//   LOG(INFO) << "LogExperimentTestFunction exit";
// #endif
// }

// void setup() {  // NOLINT
//   // Setup serial, wait for it to be ready so that our logging messages can be
//   // read.
//   Serial.begin(9600);
//   // Wait for serial port to connect, or at least some minimum amount of time
//   // (TBD), else the initial output gets lost.
//   while (!Serial) {
//   }

//   Serial.println("calling test function");
//   LogExperimentTestFunction();
// }

// void loop() {  // NOLINT
//   Serial.println("loop entry");
//   delay(1000);
// }

// #endif  // TAS_ENABLE_LOG_EXPERIMENT

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LOG_EXPERIMENT_H_
