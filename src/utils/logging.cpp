#include "utils/logging.h"

#include "utils/platform.h"

namespace alpaca {

void announceFailure(const Printable& message) {
  while (true) {
    Serial.println();
    message.printTo(Serial);
    delay(1000);
  }
}

}  // namespace alpaca
