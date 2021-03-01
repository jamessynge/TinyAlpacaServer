#include "literal.h"

#include "escaping.h"
#include "logging.h"
#include "platform.h"

namespace alpaca {
namespace {
// Returns the char at the specified location in PROGMEM.
inline char pgm_read_char(PGM_P ptr) {
  auto byte = pgm_read_byte(reinterpret_cast<const uint8_t*>(ptr));
  return static_cast<char>(byte);
}
}  // namespace

// Returns true if the other literal has the same value.
bool Literal::operator==(const Literal& other) const {
  if (size_ == other.size_) {
    if (ptr_ == other.ptr_) {
      return true;
    }
    for (size_type offset = 0; offset < size_; ++offset) {
      if (at(offset) != other.at(offset)) {
        return false;
      }
    }
    return true;
  }
  return false;
}

bool Literal::same(const Literal& other) const {
  return ptr_ == other.ptr_ && size_ == other.size_;
}

char Literal::at(const Literal::size_type pos) const {
  TAS_DCHECK_LT(pos, size_, "");
  return pgm_read_char(ptr_ + pos);
}

bool Literal::equal(const char* other, size_type other_size) const {
  if (size_ != other_size) {
    return false;
  }
  return 0 == memcmp_P(ptr_, other, size_);
}

bool Literal::case_equal(const char* other, size_type other_size) const {
  if (size_ != other_size) {
    return false;
  }
  return 0 == strncasecmp_P(ptr_, other, size_);
}

bool Literal::is_prefix_of(const char* other, size_type other_size) const {
  if (size_ > other_size) {
    // Can't be a prefix of a shorter string.
    return false;
  }
  return 0 == memcmp_P(ptr_, other, size_);
}

bool Literal::copyTo(char* out, size_type size) {
  if (size < size_) {
    return false;
  }
  memcpy_P(out, ptr_, size_);
  return true;
}

size_t Literal::printTo(Print& out) const {
  // Not particularly efficient, but probably OK. If not, use memcpy_P to copy
  // in sequential chunks of the literal into a small stack allocated buffer
  // (e.g. char buffer[16]), printing the buffer's contents after each copy
  // operation.
  size_t total = 0;
  for (size_type offset = 0; offset < size_; ++offset) {
    char c = pgm_read_char(ptr_ + offset);
    total += out.print(c);
  }
  return total;
}

size_t Literal::printJsonEscapedTo(Print& out) const {
  size_t total = out.print('"');
  for (size_type offset = 0; offset < size_; ++offset) {
    char c = pgm_read_char(ptr_ + offset);
    total += PrintCharJsonEscaped(out, c);
  }
  total += out.print('"');
  return total;
}

JsonLiteral Literal::escaped() const { return JsonLiteral(*this); }

JsonLiteral::JsonLiteral(const Literal& literal) : literal_(literal) {}

size_t JsonLiteral::printTo(Print& p) const {
  return literal_.printJsonEscapedTo(p);
}

#if TAS_HOST_TARGET
class PrintableLiteral : public Printable {
 public:
  explicit PrintableLiteral(const Literal& literal) : literal_(literal) {}

  size_t printTo(Print& p) const override { return literal_.printTo(p); }

 private:
  const Literal literal_;
};

std::ostream& operator<<(std::ostream& out, const Literal& literal) {
  return out << PrintableLiteral(literal);
}
#endif

}  // namespace alpaca
