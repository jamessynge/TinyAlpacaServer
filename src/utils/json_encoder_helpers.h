#ifndef TINY_ALPACA_SERVER_SRC_UTILS_JSON_ENCODER_HELPERS_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_JSON_ENCODER_HELPERS_H_

// Adds support for encoding various arrays and objects as JSON, building on the
// basic JsonElementSource and JsonPropertySource.

#include "utils/json_encoder.h"
#include "utils/platform.h"

namespace alpaca {

template <typename T, typename U>
class ArrayViewSource : public JsonElementSource {
 public:
  using AddToFunction = void (JsonArrayEncoder::*)(U);

  ArrayViewSource(ArrayView<T> values, AddToFunction add_to_function)
      : values_(values), add_to_function_(add_to_function) {}

  template <size_t N>
  ArrayViewSource(const T (&values)[N], AddToFunction add_to_function)
      : ArrayViewSource(ArrayView<const T>(values, N), add_to_function) {}

  void AddTo(JsonArrayEncoder& encoder) const override {
    for (auto value : values_) {
      (encoder.*(add_to_function_))(value);
    }
  }

 private:
  ArrayView<T> values_;
  AddToFunction add_to_function_;
};

template <typename T, int N, typename U>
ArrayViewSource<T, U> MakeArrayViewSource(
    T (&values)[N], void (JsonArrayEncoder::*add_to_function)(U)) {
  return ArrayViewSource<T, U>(MakeArrayView(values), add_to_function);
}

template <typename T, typename U = T>
ArrayViewSource<T, U> MakeArrayViewSource(
    ArrayView<T> values, void (JsonArrayEncoder::*add_to_function)(U)) {
  return ArrayViewSource<T, U>(values, add_to_function);
}

// JsonElementSourceAdapter allows an object that can't have a virtual function
// table (e.g. it is constexpr and maybe stored in PROGMEM) to act as a
// JsonElementSource.
template <class T>
class JsonElementSourceAdapter : public JsonElementSource {
 public:
  explicit JsonElementSourceAdapter(const T& wrapped) : wrapped_(wrapped) {}
  void AddTo(JsonArrayEncoder& encoder) const override {
    wrapped_.AddTo(encoder);
  }

 private:
  const T& wrapped_;
};

// JsonPropertySourceAdapter allows an object that can't have a virtual function
// table (e.g. it is constexpr and maybe stored in PROGMEM) to act as a
// JsonPropertySource.
template <class T>
class JsonPropertySourceAdapter : public JsonPropertySource {
 public:
  explicit JsonPropertySourceAdapter(const T& wrapped) : wrapped_(wrapped) {}
  void AddTo(JsonObjectEncoder& encoder) const override {
    wrapped_.AddTo(encoder);
  }

 private:
  const T& wrapped_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_JSON_ENCODER_HELPERS_H_
