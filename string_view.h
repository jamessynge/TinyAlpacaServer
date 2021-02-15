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
  constexpr StringView(const char (&buf)[N])  // NOLINT
      : ptr_(buf), size_(N - 1) {
    // Can't leave this DLOG uncommented, else the ctor won't be a constexpr.
    // DLOG(INFO) << "StringView literal string ctor for \"" << buf << "\"";
  }

  // Construct with a specified length.
  explicit StringView(const char* ptr, size_type length)
      : ptr_(ptr), size_(length) {}

#if ALPACA_DECODER_HAVE_STD_STRING
  // Construct from a std::string. This is used for tests.
  explicit StringView(const std::string& str)
      : ptr_(str.data()), size_(str.size()) {
    CHECK_LE(str.size(), kMaxSize);
  }
#endif

  // Copy constructor.
  constexpr StringView(const StringView& other) = default;

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
    size_ -= prefix.size_;
    ptr_ += prefix.size_;
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

  constexpr bool operator==(const StringView& other) const {
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

  constexpr bool operator!=(const StringView& other) const {
    return !(*this == other);
  }

  const_iterator begin() const { return ptr_; }
  const_iterator end() const { return ptr_ + size_; }

  // Similar to find, but just returns true if c is in this view.
  // TODO(jamessynge): Do we need this at all? If so, should we delegate the
  // looping to find(char)?
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

  bool contains(const StringView& other) const {
    return find(other) != kMaxSize;
  }

  // Returns true if this ends_with with s.
  constexpr bool ends_with(const StringView& s) const {
    if (s.size_ > size_) {
      return false;
    }
    return suffix(s.size_) == s;
  }

  // Returns true if this ends_with with c.
  constexpr bool ends_with(const char c) const {
    return size_ > 0 && ptr_[size_ - 1] == c;
  }

  // Find the first occurrence of c in this view, or kMaxSize if not found.
  size_type find(char c) const {
    for (int ndx = 0; ndx < size_; ++ndx) {
      if (ptr_[ndx] == c) {
        return ndx;
      }
    }
    return kMaxSize;
  }

  // Find the first occurrence of other in this view, or kMaxSize if not
  // found.
  size_type find(const StringView& other) const {
    if (other.size() <= size()) {
      const size_type limit = size() - other.size();
      const char c = other.at(0);
      size_type ndx = find(c);
      while (ndx <= limit) {
        if (suffix(ndx).starts_with(other)) {
          return ndx;
        }
        ndx = suffix(ndx + 1).find(c);
      }
    }
    return kMaxSize;
  }

  // Compares this view with other (converted to lower-case ASCII), returning
  // true if they are equal. This means that this view must contain no
  // upper-case letters, else it will never match other.
  bool matches_lower(const StringView& other) const {
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

  // Compares this view (which MUST be lower-case ASCII, i.e. contains
  // characters in the set 'a' to 'z', and nothing else) with other (converted
  // to lower-case ASCII), returning true if they are equal.
  bool matches_unsafe_lower(const StringView& other) const {
    if (size_ != other.size()) {
      return false;
    }
    for (StringView::size_type pos = 0; pos < size_; ++pos) {
      const char our_lc_char = ptr_[pos];
      DCHECK(absl::ascii_islower(our_lc_char)) << substr(pos, 1);
      // Here comes the unsafe part. For example, if our_lc_char is '^', it
      // won't match a '^' in view, etc.
      const char other_lc_char = other.at(pos) | static_cast<char>(0x20);
      if (our_lc_char != other_lc_char) {
        return false;
      }
      DCHECK(absl::ascii_isupper(other.at(pos)) ||
             absl::ascii_islower(other.at(pos)))
          << other.substr(pos, 1);
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
  // Non-mutating methods which create something new or perform a conversion.

  // Returns a view of a portion of this view (at offset `pos` and length `n`)
  // as another StringView. Does NOT validate the parameters, so pos+n must not
  // be greater than length().
  constexpr StringView substr(size_type pos, size_type n) const {
    DCHECK_LE(pos, size_);
    DCHECK_LE(pos + n, size_);
    return StringView(ptr_ + pos, n);
  }

  // Returns a view of a portion of this view (at offset `pos` and length `n`)
  // as another StringView. Does NOT validate the parameters, so pos+n must not
  // be greater than length().
  constexpr StringView prefix(size_type n) const {
    DCHECK_LE(n, size_);
    return StringView(ptr_, n);
  }

  // Returns a suffix "substring" of this view starting at offset `pos` and
  // continuing for the rest of the string. Does NOT validate the parameters, so
  // pos must not be greater than length().
  constexpr StringView suffix(size_type pos) const {
    DCHECK_LE(pos, size_);
    return StringView(ptr_ + pos, size_ - pos);
  }

  bool to_uint32(uint32_t& out) const;



#if ALPACA_DECODER_HAVE_STD_STRING_VIEW
  std::string_view ToStdStringView() const {
    return std::string_view(data(), size());
  }
#endif
#if ALPACA_DECODER_HAVE_STD_STRING
  std::string ToString() const { return std::string(data(), size()); }
  std::string ToEscapedString() const;
#endif

 private:
  const char* ptr_;
  size_type size_;
};

// Need this operator for CHECK_EQ, et al.
#if ALPACA_DECODER_HAVE_STD_STRING_VIEW
std::ostream& operator<<(std::ostream& out, StringView view);
#endif
#if ALPACA_DECODER_HAVE_STD_STRING
bool operator==(const StringView& a, const std::string& b);
bool operator==(const std::string& a, const StringView& b);
#endif

}  // namespace alpaca

#endif  // ALPACA_DECODER_STRING_VIEW_H_
