#define MCU_ENABLED_VLOG_LEVEL 2

#include <Arduino.h>
#include <TinyAlpacaServer.h>

// Measure switch bounces. Could be done with an oscilloscope, but not everybody
// has one of those.

#define kCoverOpenLimitPin 20
#define kCoverCloseLimitPin 21

// #define RECORD_RISING_AND_FALLING
#define RECORD_CHANGE

namespace {

// Forward declarations

#ifdef RECORD_RISING_AND_FALLING
void OnFalling();
void OnRising();
#elif defined(RECORD_CHANGE)
void OnChange();
#endif

class EventInfo {
 public:
  EventInfo(const char* name, uint8_t pin)
      : name_(name), pin_(pin), interrupt_(digitalPinToInterrupt(pin)) {}

  void Reset() {
    MCU_VLOG(1) << MCU_FLASHSTR("\nEventInfo::Reset\n");

    pinMode(pin_, INPUT);
    noInterrupts();
#ifdef RECORD_RISING_AND_FALLING
    bool awaiting_falling;
    while (true) {
      auto value1 = digitalRead(pin_);
      if (value1 == LOW) {
        attachInterrupt(interrupt_, OnRising, RISING);
      } else {
        attachInterrupt(interrupt_, OnFalling, FALLING);
      }
      auto value2 = digitalRead(pin_);
      if (value1 == value2) {
        awaiting_falling = value2 == HIGH;
        break;
      }
    }
#elif defined(RECORD_CHANGE)
    attachInterrupt(interrupt_, OnChange, CHANGE);
#else
#error "Don't know what to do."
#endif

    count_ = 0;
    missed_count_ = 0;
    interrupts();

#ifdef RECORD_RISING_AND_FALLING
    if (awaiting_falling) {
      MCU_VLOG(1) << MCU_FLASHSTR("Awaiting ") << name_
                  << MCU_FLASHSTR("pin FALLING");
    } else {
      MCU_VLOG(1) << MCU_FLASHSTR("Awaiting pin RISING");
    }
#elif defined(RECORD_CHANGE)
    MCU_VLOG(1) << MCU_FLASHSTR("Awaiting pin CHANGE");
#endif
  }

#ifdef RECORD_RISING_AND_FALLING
  void OnPinFalling() {
    attachInterrupt(interrupt_, OnRising, RISING);
    RecordEvent(true);
  }

  void OnPinRising() {
    attachInterrupt(interrupt_, OnFalling, FALLING);
    RecordEvent(false);
  }
#elif defined(RECORD_CHANGE)
  void OnPinChange() { RecordEvent(digitalRead(pin_) == LOW); }
#endif

  // If we've recorded events, and things have stabilized, then report the
  // results.
  void Loop() {
    auto count = count_;
    if (count == 0) {
      return;
    }

    // Have things stabilized?
    auto micros_since_last_event = micros() - event_micros_[count - 1];
    if (micros_since_last_event < 100000) {
      // Not necessarily.
      return;
    }

    if (count > kMaxEvents) {
      noInterrupts();
      auto missed_count = missed_count_;
      interrupts();
      MCU_VLOG(1) << MCU_FLASHSTR("There were ") << missed_count
                  << MCU_FLASHSTR(" events that we didn't record.");
    }

    MCU_VLOG(1) << MCU_FLASHSTR("Recorded ") << count
                << MCU_FLASHSTR(" events.");
    MCU_VLOG(1) << (event_is_falling_[0] ? MCU_FLASHSTR("Falling")
                                         : MCU_FLASHSTR("Rising"))
                << MCU_FLASHSTR(" @ micros ") << event_micros_[0];
    for (uint8_t ndx = 1; ndx < count; ++ndx) {
      MCU_VLOG(1) << '+' << (event_micros_[ndx] - event_micros_[ndx - 1])
                  << MCU_FLASHSTR(" micros later was ")
                  << (event_is_falling_[ndx] ? MCU_FLASHSTR("Falling")
                                             : MCU_FLASHSTR("Rising"))
                  << MCU_FLASHSTR(" @ micros ") << event_micros_[ndx];
    }

    Reset();
  }

 private:
  void RecordEvent(bool falling) {
    if (count_ >= kMaxEvents) {
      ++missed_count_;
    } else {
      event_micros_[count_] = micros();
      event_is_falling_[count_] = falling;
      ++count_;
    }
  }

  static constexpr uint8_t kMaxEvents = 100;
  volatile uint8_t count_;
  volatile bool event_is_falling_[kMaxEvents];
  volatile uint32_t event_micros_[kMaxEvents];
  volatile uint16_t missed_count_;
  const char* const name_;
  const uint8_t pin_;
  const uint8_t interrupt_;
};

EventInfo event_info("Open Limit", kCoverOpenLimitPin);

#ifdef RECORD_RISING_AND_FALLING
void OnFalling() { event_info.OnPinFalling(); }
void OnRising() { event_info.OnPinRising(); }
#elif defined(RECORD_CHANGE)
void OnChange() { event_info.OnPinChange(); }
#endif

}  // namespace

void setup() {
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read. Note that the baud rate is meaningful on boards that do true serial,
  // while those microcontrollers with builtin USB likely don't rate limit
  // because there isn't a need.
  Serial.begin(115200);

  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost. Note that this isn't true for all
  // Arduino-like boards: some reset when the Serial Monitor connects, so we
  // almost always get the initial output.
  while (!Serial) {
  }

  event_info.Reset();
}

void loop() { event_info.Loop(); }
