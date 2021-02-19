#ifndef ALPACA_DECODER_STRING_VIEW_H_
#define ALPACA_DECODER_STRING_VIEW_H_

// Author: james.synge@gmail.com

// StringView supports referencing subsequences of a string without making any
// copies. Does not support modifying the underlying string.
//
// Inspired by and based on std::string_view and absl::string, with copies of
// some of the comments and definitions. However, since an embedded system
// doesn't have much logging or debugging support, the caller needs to know what
// they're doing. So we have some DCHECKS, but we don't otherwise prevent
// invalid requests, including not throwing any exceptions. Callers should use
// extensive testing, including fuzz testing, to find bugs.
//
// One constructor and a number of methods are specified as constexpr to
// encourage the compiler to perform compile time operations instead of waiting
// until runtime.

#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <string_view>

#include "absl/strings/ascii.h"
#include "alpaca-decoder/config.h"
#include "alpaca-decoder/logging.h"

namespace alpaca {

class StringView {
 public:
  using size_type = uint8_t;
  using const_iterator = const char*;
  using iterator = const_iterator;
  static constexpr size_type kMaxSize = std::numeric_limits<size_type>::max();

  // Construct empty.
  constexpr StringView() noexcept : ptr_(nullptr), size_(0) {}

  // Constructs from a literal string (e.g. constexpr char abc[] = "abc"). The
  // goal of this is to get the compiler to populate the size, rather than
  // computing it at runtime.
  // NOTE: The length of a literal string includes the NUL (\0) at the end,
  // so we subtract one from N to get the length of the string before that.
  // NOTE: There is no (const char* ptr) constructor because it blocks use of
  // this constructor for literals.
  template <size_type N>
  ALPACA_CONSTEXPR_FUNC StringView(const char (&buf)[N])  // NOLINT
      : ptr_(buf), size_(N - 1) {
#ifndef NDEBUG
    DVLOG(7) << "StringView literal string ctor for \"" << buf << "\"";
#endif
  }

  // Construct with a specified length.
  ALPACA_CONSTEXPR_FUNC StringView(const char* ptr, size_type length)
      : ptr_(ptr), size_(length) {}

#if ALPACA_DECODER_HAVE_STD_STRING
  // Construct from a std::string. This is used for tests.
  explicit StringView(const std::string& str)
      : ptr_(str.data()), size_(str.size()) {
    DCHECK_LE(str.size(), kMaxSize);
  }
#endif

  // Copy constructor.
  ALPACA_CONSTEXPR_FUNC StringView(const StringView& other) = default;

  //////////////////////////////////////////////////////////////////////////////
  // Mutating methods:

  // Consider whether to remove this method, thus ensuring that a StringView
  // instance can never grow to a larger size.
  StringView& operator=(const StringView& other) = default;

  // Returns true if this starts with prefix, in which case it removes that
  // prefix.
  bool match_and_consume(const StringView& prefix) {
    if (!starts_with(prefix)) {
      return false;
    }
    // Using this order just in case this object is the prefix object.
    ptr_ += prefix.size_;
    size_ -= prefix.size_;
    return true;
  }

  // Remove the first prefix_length characters from the StringView.
  void remove_prefix(size_type prefix_length) noexcept {
    DCHECK_LE(prefix_length, size_);
    size_ -= prefix_length;
    ptr_ += prefix_length;
  }

  // Remove the last suffix_length characters from the StringView.
  void remove_suffix(size_type suffix_length) {
    DCHECK_LE(suffix_length, size_);
    size_ -= suffix_length;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-mutating methods:

  ALPACA_CONSTEXPR_FUNC bool operator==(const StringView& other) const {
#ifndef NDEBUG
    VLOG(7) << "StringView::operator==(" << ToEscapedString() << ", "
            << other.ToEscapedString() << ")";
#endif  // !NDEBUG
    if (other.size_ != size_) {
      return false;
    }
    auto ndx = size_;
    while (ndx > 0) {
      --ndx;
      if (ptr_[ndx] != other.ptr_[ndx]) {
        return false;
      }
    }
    return true;
  }

  bool operator==(const char* other) const {
    if (other == nullptr) {
      return empty();
    }
    for (size_type ndx = 0; ndx < size_ && *other != '\0'; ++ndx, ++other) {
      if (at(ndx) != *other) {
        return false;
      }
    }
    return *other == '\0';
  }

  constexpr bool operator!=(const StringView& other) const {
    return !(*this == other);
  }

  const_iterator begin() const { return ptr_; }
  const_iterator end() const { return ptr_ + size_; }
  char front() const {
    DCHECK(!empty());
    return *ptr_;
  }
  char back() const {
    DCHECK(!empty());
    return ptr_[size_ - 1];
  }

  // Returns true if this view contains c.
  bool contains(char c) const {
    // Consider whether to use memchr.
    // return memchr(ptr_, c, size_) != nullptr;
    for (int ndx = 0; ndx < size_; ++ndx) {
      if (ptr_[ndx] == c) {
        return true;
      }
    }
    return false;
  }

  // Returns true if this view contains the other as a substring.
  // This is only used in one place in the decoder, and probably only for
  // PUT requests, so making this as simple as possible.
  bool contains(const StringView& other) const {
    StringView copy(*this);
    while (copy.size() >= other.size()) {
      if (copy.starts_with(other)) {
        return true;
      }
      copy.remove_prefix(1);
    }
    return false;
  }

  // Returns true if this ends_with with c.
  constexpr bool ends_with(const char c) const {
    return size_ > 0 && ptr_[size_ - 1] == c;
  }

  // Compares this view with other (converted to lower-case ASCII), returning
  // true if they are equal. This means that this view must contain no
  // upper-case letters, else it will never match other.
  bool equals_other_lowered(const StringView& other) const {
    if (size_ != other.size()) {
      return false;
    }
    for (StringView::size_type pos = 0; pos < size_; ++pos) {
      const char our_lc_char = ptr_[pos];
      DCHECK(!absl::ascii_isupper(our_lc_char)) << substr(pos, 1);
      const char other_char = other.at(pos);
      const char other_lc_char = absl::ascii_isupper(other_char)
                                     ? (other_char | static_cast<char>(0x20))
                                     : other_char;
      if (our_lc_char != other_lc_char) {
        return false;
      }
    }
    return true;
  }

  // Returns true if this starts with s.
  constexpr bool starts_with(const StringView& s) const {
    if (s.size_ > size_) {
      return false;
    }
    return prefix(s.size_) == s;
  }

  // Returns true if this starts with c.
  constexpr bool starts_with(const char c) const {
    return size_ > 0 && *ptr_ == c;
  }

  // Returns the number of characters in the StringView. Alias for length.
  constexpr size_type size() const { return size_; }

  // Returns true if there are no characters in the view.
  constexpr bool empty() const { return size_ == 0; }

  // Returns a pointer to the underlying character array (which is of course
  // stored elsewhere). Note that the underlying character array may contain
  // embedded NUL characters, and may or may not be NUL-terminated. Therefore,
  // do not pass `data()` to a routine that expects a NUL-terminated string
  // unless you otherwise know that it is properly terminated.
  constexpr const char* data() const { return ptr_; }

  constexpr char at(size_type pos) const {
    DCHECK_LT(pos, size_);
    return ptr_[pos];
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-mutating methods which perform a conversion or return a new object.

  // Returns a view of a portion of this view (at offset `pos` and length `n`)
  // as another StringView. Does NOT validate the parameters, so pos+n must not
  // be greater than length(). This is currently only used for tests.
  ALPACA_CONSTEXPR_FUNC StringView substr(size_type pos, size_type n) const {
#ifndef NDEBUG
    DCHECK_LE(pos, size_);
    DCHECK_LE(pos + n, size_);
#endif  // NDEBUG
    return StringView(ptr_ + pos, n);
  }

  // Returns a new StringView containing the first n characters of this view.
  constexpr StringView prefix(size_type n) const {
    if (n >= size_) {
      return *this;
    } else {
      return StringView(ptr_, n);
    }
  }

  // Returns a new StringView containing the last n characters of this view.
  constexpr StringView suffix(size_type n) const {
    if (n >= size_) {
      return *this;
    } else {
      return StringView(ptr_ + (size_ - n), n);
    }
  }

  bool to_uint32(uint32_t& out) const;

  // The following methods are for testing and debugging on a "real" computer,
  // not for the embedded device.

#if ALPACA_DECODER_HAVE_STD_STRING_VIEW
  // Convert to std::string_view.
  std::string_view ToStdStringView() const;
#endif

#if ALPACA_DECODER_HAVE_STD_STRING
  std::string ToString() const { return std::string(data(), size()); }
  std::string ToEscapedString() const;
#endif

 private:
  const char* ptr_;
  size_type size_;
};

// The streaming and equals operators are used for tests, CHECK_EQ, etc. They
// aren't used by the NDEBUG (i.e. embedded/production) portion of the decoder.

#if ALPACA_DECODER_HAVE_STD_OSTREAM
std::ostream& operator<<(std::ostream& out, StringView view);
#endif  // ALPACA_DECODER_HAVE_STD_OSTREAM

#if ALPACA_DECODER_HAVE_STD_STRING_VIEW
bool operator==(const StringView& a, std::string_view b);
#endif  // ALPACA_DECODER_HAVE_STD_STRING_VIEW

#if ALPACA_DECODER_HAVE_STD_STRING
bool operator==(const StringView& a, const std::string& b);

bool operator==(const std::string& a, const StringView& b);
#endif  // ALPACA_DECODER_HAVE_STD_STRING

}  // namespace alpaca

#endif  // ALPACA_DECODER_STRING_VIEW_H_
