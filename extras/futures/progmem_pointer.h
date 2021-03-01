#ifndef TINY_ALPACA_SERVER_EXTRAS_FUTURES_PROGMEM_POINTER_H_
#define TINY_ALPACA_SERVER_EXTRAS_FUTURES_PROGMEM_POINTER_H_

// In support of iterating over strings, and possibly other structures, this
// helps to encapsulate some of the challenges of dealing with data stored in
// PROGMEM.
//
// NOTE: This only addresses (hah) "near" addresses, i.e. in the first 64KB.
//
// Learn more about writing custom iterators here:
//    https://internalpointers.com/post/writing-custom-iterators-modern-cpp

#include "platform.h"
namespace alpaca {

template <typename T, typename P = PGM_P>
class ProgMemPtr {
 public:
  using value_type = typename T::value_type;
  using reference_type = value_type&;
  using pointer_type = value_type*;
  using unit_array_type = value_type[1];
  static constexpr size_t kStepSize = sizeof(unit_array_type);

  ProgMemPtr() : ProgMemPtr(0) {}
  explicit ProgMemPtr(P ptr) : ptr_(ptr), loaded_temp_(false) {}

  const reference_type operator*() {
    load_temp();
    return temp_;
  }

  pointer_type operator->() {
    load_temp();
    return &temp_;
  }

  // Prefix increment
  ProgMemPtr& operator++() {
    ptr_ += kStepSize;
    return *this;
  }

  // Postfix increment
  ProgMemPtr operator++(int) {
    ProgMemPtr tmp = *this;
    ++(*this);
    return tmp;
  }

 private:
  void load_temp() {
    if (!loaded_temp_) {
      if (sizeof temp_ == 1) {
        temp_ = reinterpret_cast<value_type>(pgm_read_byte(ptr_));
      } else if (sizeof temp_ == 2) {
        temp_ = reinterpret_cast<value_type>(pgm_read_word(ptr_));
      } else if (sizeof temp_ == 4) {
        temp_ = reinterpret_cast<value_type>(pgm_read_dword(ptr_));
      } else {
        memcpy_P(&temp_, ptr_, sizeof temp_);
      }
      loaded_temp_ = true;
    }
  }

  P ptr_;
  value_type temp_;
  bool loaded_temp_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_FUTURES_PROGMEM_POINTER_H_
