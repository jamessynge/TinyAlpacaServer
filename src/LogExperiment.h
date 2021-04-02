#ifndef SRC_LOGEXPERIMENT_H
#define SRC_LOGEXPERIMENT_H

// This is really a test of how well the compiler and linker can handle NOT
// emitting reachable code into the final binary. This file is included from
// sketches TestLogExperimentDisabled and TestLogExperimentEnabled.

#if defined(TAS_ENABLE_INCLUDE_LOGGING_EXPERIMENT) || \
    defined(TAS_ENABLE_LOG_EXPERIMENT) ||             \
    defined(TAS_ENABLE_CHECK_EXPERIMENT) ||           \
    defined(TAS_ENABLE_DCHECK_EXPERIMENT)

#include "utils/logging.h"
#include "utils/platform.h"

void setup() {  // NOLINT
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read.
  Serial.begin(9600);
  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost.
  while (!Serial) {
  }

#ifdef TAS_ENABLE_LOG_EXPERIMENT
  TAS_VLOG(1) << "Serial is ready";
  if (false)
    ;
  else  // NOLINT: Checking the else behavior.
    TAS_VLOG(1) << "An integer " << 1 << ", followed by a float " << 3.14;

  if (true)
    TAS_VLOG(1) << "An integer " << 1 << ", followed by a float " << 3.14;
  else {  // NOLINT: Checking the if behavior.
    TAS_VLOG(1) << "Should NOT be executed";
  }
#endif

#ifdef TAS_ENABLE_CHECK_EXPERIMENT
  if (true)
    TAS_CHECK(true) << "Should NOT be printed";
  else
    Serial.println("Should NOT be printed.");
  if (false) {
  } else  // NOLINT: Checking the else behavior.
    TAS_CHECK(false) << "Should NOT be printed";
#endif

#ifdef TAS_ENABLE_DCHECK_EXPERIMENT
  if (true)
    TAS_DCHECK(true) << "Should NOT be printed";
  else
    Serial.println("Should NOT be printed.");
  if (false) {
  } else  // NOLINT: Checking the else behavior.
    TAS_DCHECK(true) << "Should NOT be printed";
#endif
}

void loop() {  // NOLINT
  Serial.println("loop entry");
  delay(1000);
}

#endif  // TAS_ENABLE_INCLUDE_LOGGING_EXPERIMENT or TAS_ENABLE_LOG_EXPERIMENT or
        // TAS_ENABLE_CHECK_EXPERIMENT or TAS_ENABLE_DCHECK_EXPERIMENT

#endif  // SRC_LOGEXPERIMENT_H
