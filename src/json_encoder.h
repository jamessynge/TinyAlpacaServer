#ifndef TINY_ALPACA_SERVER_JSON_ENCODER_H_
#define TINY_ALPACA_SERVER_JSON_ENCODER_H_

// Supports writing a JSON object, with values that are numbers, bools, strings,
// and arrays of the same. Usage examples can be // found in the test file.
//
// Author: james.synge@gmail.com

#include <stddef.h>

#include <cmath>
#include <type_traits>

#include "host_printable.h"
#include "logging.h"
#include "string_view.h"

namespace alpaca {
namespace internal {

// Writes a string literal to out.
// NOTE: The length of a literal string includes the NUL (\0) at the end, so we
// subtract one from N to get the length of the string before that.

template <size_t N>
inline void PrintStringLiteral(Print& out, const char (&buf)[N]) {
  out.write(buf, N - 1);
}

template <typename T>
void PrintInteger(Print& out, const T value) {
  // +0 to cause promotion of chars, if they're passed in.
  out.print(value + 0);
}

// Prints the floating point value to out, if possible. If not, prints a JSON
// string that "describes" the situation. This is similar to various JSON
// libraries, on which this is based.
template <typename T>
void PrintFloatingPoint(Print& out, const T value) {
  // We're assuming that the stream is configured to match JSON requirements for
  // the formatting of numbers.
  if (std::isnan(value)) {
    JsonStringView("NaN").printTo(out);
  } else if (!std::isfinite(value)) {
    StringView v("-Inf");
    if (value > 0) {
      v.remove_prefix(1);
    }
    v.escaped().printTo(out);
  } else {
    out.print(value);
  }
}

// Prints true or false to out.
void PrintBoolean(Print& out, const bool value);

}  // namespace internal

class JsonArrayEncoder;
class JsonObjectEncoder;

// Base class for the object and array encoders.
class AbstractJsonEncoder {
 protected:
  explicit AbstractJsonEncoder(Print& out, AbstractJsonEncoder* parent)
      : out_(out),
#if ALPACA_SERVER_DEBUG
        parent_(parent),
        has_child_(false),
        is_live_(true),
#endif
        first_(true) {
#if ALPACA_SERVER_DEBUG
    if (parent != nullptr) {
      parent->StartChild();
    }
#endif
  }

  AbstractJsonEncoder(const AbstractJsonEncoder&) = delete;
  AbstractJsonEncoder(AbstractJsonEncoder&& other)
      : out_(other.out_),
#if ALPACA_SERVER_DEBUG
        parent_(other.parent_),
        has_child_(other.has_child_),
#endif
        first_(true) {
#if ALPACA_SERVER_DEBUG
    DCHECK(other.is_live_);
    other.parent_ = nullptr;
    other.is_live_ = false;
#endif
  }

  AbstractJsonEncoder& operator=(const AbstractJsonEncoder&) = delete;
  AbstractJsonEncoder& operator=(AbstractJsonEncoder&&) = delete;

  ~AbstractJsonEncoder() {
#if ALPACA_SERVER_DEBUG
    DCHECK(!has_child_);
    if (is_live_) {
      if (parent_ != nullptr) {
        parent_->EndChild();
      }
    } else {
      DCHECK_EQ(parent_, nullptr);
    }
#endif
  }

  void StartItem() {
#if ALPACA_SERVER_DEBUG
    DCHECK(is_live_);
#endif
    if (first_) {
      first_ = false;
    } else {
      internal::PrintStringLiteral(out_, ", ");
    }
  }

  JsonArrayEncoder MakeChildArrayEncoder();
  JsonObjectEncoder MakeChildObjectEncoder();

  Print& out_;

 private:
#if ALPACA_SERVER_DEBUG
  void StartChild() {
    DCHECK(!has_child_);
    has_child_ = true;
  }

  void EndChild() {
    DCHECK(has_child_);
    has_child_ = false;
  }

  AbstractJsonEncoder* parent_;
  bool has_child_;
  bool is_live_;
#endif
  bool first_;
};

// JSON encoder for objects.
class JsonObjectEncoder : public AbstractJsonEncoder {
 public:
  explicit JsonObjectEncoder(Print& out) : JsonObjectEncoder(out, nullptr) {}

  ~JsonObjectEncoder() { internal::PrintStringLiteral(out_, "}"); }

  JsonObjectEncoder(const JsonObjectEncoder&) = delete;
  JsonObjectEncoder(JsonObjectEncoder&&) = default;
  JsonObjectEncoder& operator=(const JsonObjectEncoder&) = delete;
  JsonObjectEncoder& operator=(JsonObjectEncoder&&) = delete;

  template <typename T, typename E = typename std::enable_if<
                            std::is_integral<T>::value>::type>
  void AddIntegerProperty(const StringView& name, const T value) {
    StartProperty(name);
    internal::PrintInteger(out_, value);
  }

  template <typename T, typename E = typename std::enable_if<
                            std::is_floating_point<T>::value>::type>
  void AddFloatingPointProperty(const StringView& name, T value) {
    StartProperty(name);
    internal::PrintFloatingPoint(out_, value);
  }

  void AddBooleanProperty(const StringView& name, const bool value) {
    StartProperty(name);
    internal::PrintBoolean(out_, value);
  }

  void AddStringProperty(const StringView& name, const StringView& value) {
    StartProperty(name);
    value.escaped().printTo(out_);
  }

  JsonArrayEncoder StartArrayProperty(const StringView& name);

  JsonObjectEncoder StartObjectProperty(const StringView& name);

 private:
  friend class AbstractJsonEncoder;

  JsonObjectEncoder(Print& out, AbstractJsonEncoder* parent)
      : AbstractJsonEncoder(out, parent) {
    internal::PrintStringLiteral(out_, "{");
  }

  void StartProperty(const StringView& name) {
    StartItem();
    name.escaped().printTo(out_);
    internal::PrintStringLiteral(out_, ": ");
  }
};

// JSON encoder for arrays.
class JsonArrayEncoder : public AbstractJsonEncoder {
 public:
  explicit JsonArrayEncoder(Print& out) : JsonArrayEncoder(out, nullptr) {}

  ~JsonArrayEncoder() { internal::PrintStringLiteral(out_, "]"); }

  template <typename T, typename E = typename std::enable_if<
                            std::is_integral<T>::value>::type>
  void AddIntegerElement(const T value) {
    StartItem();
    internal::PrintInteger(out_, value);
  }

  template <typename T, typename E = typename std::enable_if<
                            std::is_floating_point<T>::value>::type>
  void AddFloatingPointElement(T value) {
    StartItem();
    internal::PrintFloatingPoint(out_, value);
  }

  void AddBooleanElement(const bool value) {
    StartItem();
    internal::PrintBoolean(out_, value);
  }

  void AddStringElement(const StringView& value) {
    StartItem();
    value.escaped().printTo(out_);
  }

  JsonArrayEncoder StartArrayElement();

  JsonObjectEncoder StartObjectElement();

 private:
  friend class AbstractJsonEncoder;

  JsonArrayEncoder(Print& out, AbstractJsonEncoder* parent)
      : AbstractJsonEncoder(out, parent) {
    internal::PrintStringLiteral(out_, "[");
  }
};
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_JSON_ENCODER_H_
