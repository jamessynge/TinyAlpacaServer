#ifndef TINY_ALPACA_SERVER_STRING_VIEW_H_
#define TINY_ALPACA_SERVER_STRING_VIEW_H_

// StringView supports referencing subsequences of a string without making any
// copies. Does not support modifying the underlying string.
//
// Inspired by and based on std::string_view and absl::string_view, with copies
// of some of the comments and definitions. However, since an embedded system
// doesn't have much logging or debugging support, the caller needs to know what
// they're doing. So we have some DCHECKS, but we don't otherwise prevent
// invalid requests, including not throwing any exceptions. Callers should use
// extensive testing, including fuzz testing, to find bugs.
//
// One constructor and a number of methods are specified as constexpr to
// encourage the compiler to perform compile time operations instead of waiting
// until runtime.
//
// Note that I've been quite inconsistent about the naming convention for method
// names. They should mostly be in PascalCase, but std::string_view, etc.,
// uses in snake_case, and I've followed that convention for many methods.
//
// Author: james.synge@gmail.com

#include <stddef.h>

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

#include "config.h"
#include "logging.h"

#ifdef Arduino_h
#include <Printable.h>
#else
#include "host_printable.h"
#endif

namespace alpaca {

class JsonStringView;

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
  constexpr StringView(const char (&buf)[N])  // NOLINT
      : ptr_(buf), size_(N - 1) {}

  // Construct with a specified length.
  ALPACA_SERVER_CONSTEXPR_FUNC StringView(const char* ptr, size_type length)
      : ptr_(ptr), size_(length) {}

#if ALPACA_SERVER_HAVE_STD_STRING
  // Construct from a std::string. This is used for tests.
  explicit StringView(const std::string& str);
#endif

  // Copy constructor.
  ALPACA_SERVER_CONSTEXPR_FUNC StringView(const StringView& other) = default;

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

  bool operator==(const StringView& other) const;

  bool operator==(const char* other) const;

  bool operator!=(const StringView& other) const;

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
  bool equals_other_lowered(const StringView& other) const;

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
  // be greater than length(). This is currently only used for non-embedded
  // code, hence the DCHECKs instead of ensuring that the result is valid.
  ALPACA_SERVER_CONSTEXPR_FUNC StringView substr(size_type pos,
                                                 size_type n) const {
#if ALPACA_SERVER_DEBUG
    DCHECK_LE(pos, size_);
    DCHECK_LE(pos + n, size_);
#endif  // ALPACA_SERVER_DEBUG
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

  // Parse the string as an unsigned, 32-bit decimal integer, writing the value
  // to out. Returns true iff successful. If not successful, does not modify
  // out.
  bool to_uint32(uint32_t& out) const;

  // Returns this StringView wrapped in a JsonStringView.
  JsonStringView escaped() const;

  // Print the string to Print by calling Print::write(data(), size()). The
  // name printTo comes from Arduino's Printable::printTo, with which this is
  // mostly compatible; the exception is that it is not virtual because that
  // would prevent StringView instances being able to be constexpr
  // constructable.
  size_t printTo(Print& p) const;

#if ALPACA_SERVER_HAVE_STD_OSTREAM
  // Writes the string to the ostream.
  void WriteTo(std::ostream& out) const;
#endif  // ALPACA_SERVER_HAVE_STD_OSTREAM

  // The following methods are for testing and debugging on a "real" computer,
  // not for the embedded device.

#if ALPACA_SERVER_HAVE_STD_STRING_VIEW
  // Convert to std::string_view.
  std::string_view ToStdStringView() const;
#endif

#if ALPACA_SERVER_HAVE_STD_STRING
  std::string ToString() const;
  std::string ToHexEscapedString() const;
#endif

 private:
  const char* ptr_;
  size_type size_;
};

// JsonStringView is a simple helper to allow us to output the JSON escaped form
// of a StringView. For example.
//     StringView view = ....;
//     DCHECK_LT(view.size(), 10) << "View is too long: " << view.escaped();
class JsonStringView : public Printable {
 public:
  explicit JsonStringView(const StringView& view);

  size_t printTo(Print& p) const override;

#if ALPACA_SERVER_HAVE_STD_OSTREAM
  void WriteTo(std::ostream& out) const;
#endif  // ALPACA_SERVER_HAVE_STD_OSTREAM

  const StringView& view() const { return view_; }

  // Returns a StringView of the JSON representation of the ASCII character. An
  // empty StringView is returned if the character is not valid in JSON (e.g.
  // Ctrl-A). A reference to the character is required because for characters
  // that don't need escaping, the StringView is constructed as a length 1 view
  // of that single character.
  static StringView GetJsonEscaped(const char& c);

 private:
  const StringView view_;
};

// The insertion streaming operators (i.e. operator<<) for values of type
// StringView and JsonStringView are used for tests, DCHECK_EQ, DVLOG, etc.

#if ALPACA_SERVER_HAVE_STD_OSTREAM
std::ostream& operator<<(std::ostream& out, const StringView& view);
std::ostream& operator<<(std::ostream& out, const JsonStringView& view);
#endif  // ALPACA_SERVER_HAVE_STD_OSTREAM

// The equals operators below are used for tests, CHECK_EQ, etc., where we want
// to compare StringViews against strings from the standard library. They aren't
// used by the embedded portion of the decoder.

#if ALPACA_SERVER_HAVE_STD_STRING_VIEW
bool operator==(const StringView& a, std::string_view b);
bool operator==(std::string_view a, const StringView& b);
#endif  // ALPACA_SERVER_HAVE_STD_STRING_VIEW

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_STRING_VIEW_H_
