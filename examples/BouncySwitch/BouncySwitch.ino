#define TAS_ENABLED_VLOG_LEVEL 2

#include <Arduino.h>
#include <TinyAlpacaServer.h>

// Measure switch bounces. Could be done with an oscilloscope, but not everybody
// has one of those.

#define kCoverOpenLimitPin 20
#define kCoverCloseLimitPin 21

namespace {

constexpr uint8_t kCoverOpenInterruptNumber =
  digitalPinToInterrupt(kCoverOpenLimitPin);
constexpr uint8_t kCoverCloseInterruptNumber =
  digitalPinToInterrupt(kCoverCloseLimitPin);

// Forward declarations

void OnFalling();
void OnRising();

class EventInfo {
  public:
    EventInfo(const char* name, uint8_t pin)
      : name_(name), pin_(pin), interrupt_(digitalPinToInterrupt(pin)) {}

    void Reset() {
      TAS_VLOG(1) << "\nEventInfo::Reset\n";

      pinMode(pin_, INPUT);
      noInterrupts();
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
      count_ = 0;
      missed_count_ = 0;
      interrupts();

      if (awaiting_falling) {
        TAS_VLOG(1) << "Expecting FALLING next.";
      } else {
        TAS_VLOG(1) << "Expecting RISING next.";
      }
    }

    void OnPinFalling() {
      RecordEvent(true);
      attachInterrupt(interrupt_, OnRising, RISING);
    }

    void OnPinRising() {
      RecordEvent(false);
      attachInterrupt(interrupt_, OnFalling, FALLING);
    }

    // If we've recorded events, and things have stabilized, then report the
    // results.
    void Loop() {
      auto count = count_;
      if (count == 0) {
        return;
      }

      // Have things stabilized?
      auto micros_since_last_event = micros() - event_micros_[count];
      if (micros_since_last_event < 100000) {
        // Not necessarily.
        return;
      }

      if (count > kMaxEvents) {
        noInterrupts();
        auto missed_count = missed_count_;
        interrupts();
        TAS_VLOG(1) << "There were " << missed_count
                    << " events that we didn't record.";
      }

      TAS_VLOG(1) << "Recorded " << count << " events.";
      TAS_VLOG(1) << (event_is_falling_[0] ? "Falling" : "Rising") << " @ micros "
                  << event_micros_[0];
      for (uint8_t ndx = 1; ndx < count; ++ndx) {
        TAS_VLOG(1) << '+' << (event_micros_[ndx] - event_micros_[ndx - 1])
                    << " micros later was "
                    << (event_is_falling_[ndx] ? "Falling" : "Rising")
                    << " @ micros " << event_micros_[ndx];
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

void OnFalling() {
  event_info.OnPinFalling();
}
void OnRising() {
  event_info.OnPinRising();
}

}  // namespace

void setup() {
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read. Note that the baud rate is meaningful on boards that do true serial,
  // while those microcontrollers with builtin USB likely don't rate limit
  // because there isn't a need.
  Serial.begin(57600);

  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost. Note that this isn't true for all
  // Arduino-like boards: some reset when the Serial Monitor connects, so we
  // almost always get the initial output.
  while (!Serial) {
  }

  event_info.Reset();
}

void loop() {
  event_info.Loop();
}
