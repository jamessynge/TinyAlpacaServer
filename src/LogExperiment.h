#ifndef TINY_ALPACA_SERVER_SRC_LOGEXPERIMENT_H_
#define TINY_ALPACA_SERVER_SRC_LOGEXPERIMENT_H_

// This is really a test of how well the compiler and linker can handle NOT
// emitting reachable code into the final binary. This file is included from
// sketches TestLogExperimentDisabled and TestLogExperimentEnabled.

#ifdef TAS_DO_LOG_EXPERIMENT

#if defined(TAS_LOG_EXPERIMENT_DO_INCLUDE_LOGGING_H) || \
    defined(TAS_LOG_EXPERIMENT_DO_LOG) ||               \
    defined(TAS_LOG_EXPERIMENT_DO_CHECK) ||             \
    defined(TAS_LOG_EXPERIMENT_DO_DCHECK)
#include "utils/logging.h"
#include "utils/inline_literal.h"
#include "utils/platform.h"
#endif


void setup() {  // NOLINT
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read.
  Serial.begin(9600);
  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost.
  while (!Serial) {
  }
  Serial.print('s');
  Serial.print('e');
  Serial.print('t');
  Serial.print('u');
  Serial.println('p');

#ifdef TAS_LOG_EXPERIMENT_DO_LOG
  TAS_VLOG(1) << TASLIT("Serial is ready");

  if (true)
    TAS_VLOG(1) << TASLIT("TAS_VLOG in if branch");
  else {  // NOLINT: Checking the if behavior.
    TAS_VLOG(1) << TASLIT("TAS_VLOG should NOT be reached");
  }

  if (false) {
  } else  // NOLINT: Checking the else behavior.
    TAS_VLOG(1) << TASLIT("TAS_VLOG in else branch");

  TAS_VLOG(99) << TASLIT("TAS_VLOG(99) should be disabled");
#endif

#ifdef TAS_LOG_EXPERIMENT_DO_CHECK
  TAS_CHECK(true) << TASLIT("TAS_CHECK should NOT fail");

  if (true)
    TAS_CHECK(true) << TASLIT("TAS_CHECK should NOT fail");
  else if (false)
    TAS_CHECK(false) << TASLIT("TAS_CHECK should NOT be reached.");

  if (false) {
    TAS_CHECK(false) << TASLIT("TAS_CHECK should NOT be reached.");
  } else  // NOLINT: Checking the else behavior.
    TAS_CHECK(true) << TASLIT("TAS_CHECK should NOT fail");
#endif

#ifdef TAS_LOG_EXPERIMENT_DO_DCHECK
  TAS_DCHECK(true) << TASLIT("TAS_DCHECK should NOT fail");

  if (true)
    TAS_DCHECK(true) << TASLIT("TAS_DCHECK should NOT fail");
  else if (false)
    TAS_DCHECK(false) << TASLIT("TAS_DCHECK should NOT be reached.");

  if (false) {
    TAS_DCHECK(false) << TASLIT("TAS_DCHECK should NOT be reached.");
  } else  // NOLINT: Checking the else behavior.
    TAS_DCHECK(true) << TASLIT("TAS_DCHECK should NOT fail");
#endif

  Serial.print('d');
  Serial.print('o');
  Serial.print('n');
  Serial.println('e');
}

void loop() {  // NOLINT
}

#endif  // TAS_DO_LOG_EXPERIMENT

#endif  // TINY_ALPACA_SERVER_SRC_LOGEXPERIMENT_H_