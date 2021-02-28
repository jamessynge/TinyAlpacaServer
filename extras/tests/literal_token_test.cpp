#include "literal_token.h"

#include <cstdint>

#include "extras/tests/test_utils.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "platform.h"
#include "string_view.h"

namespace alpaca {
namespace {

constexpr char kAllLowerStr[] = "some text and 123 (numbers)";
constexpr char kMixedCaseStr[] = "Some Text And 123 (Numbers)";
constexpr char kAllUpperStr[] = "SOME TEXT AND 123 (NUMBERS)";

TAS_CONSTEXPR_VAR Literal kAllLowerLiteral(kAllLowerStr);
TAS_CONSTEXPR_VAR Literal kMixedCaseLiteral(kMixedCaseStr);
TAS_CONSTEXPR_VAR Literal kAllUpperLiteral(kAllUpperStr);

constexpr uint8_t kAllLowerId = 123;
constexpr uint8_t kMixedCaseId = 99;
constexpr uint8_t kAllUpperId = 231;

TAS_CONSTEXPR_VAR LiteralToken<uint8_t> kLMUTokens[] = {
    {kAllLowerLiteral, kAllLowerId},
    {kMixedCaseLiteral, kMixedCaseId},
    {kAllUpperLiteral, kAllUpperId},
};

TAS_CONSTEXPR_VAR LiteralToken<uint8_t> kUMLTokens[] = {
    {kAllUpperLiteral, kAllUpperId},
    {kMixedCaseLiteral, kMixedCaseId},
    {kAllLowerLiteral, kAllLowerId},
};

struct LitCase {
  const bool matches;
  uint8_t match_id;
};

struct LitCases {
  const char* char_ptr;
  LitCase exact;
  LitCase caseless;
};

TEST(LiteralTokenTest, Mismatch) {
  // No match for an empty string.
  {
    uint8_t matched_id = 0;
    EXPECT_FALSE(
        MaybeMatchLiteralTokensExactly(StringView(""), kLMUTokens, matched_id));
    EXPECT_EQ(matched_id, 0);
    EXPECT_FALSE(MaybeMatchLiteralTokensCaseInsensitively(
        StringView(""), kLMUTokens, matched_id));
    EXPECT_EQ(matched_id, 0);
    EXPECT_EQ(matched_id, 0);
  }
  // Doesn't match a string of the same length, but different content.
  {
    const std::string same_length(kAllLowerLiteral.size(), 'x');
    StringView view(same_length);
    EXPECT_EQ(view.size(), kAllLowerLiteral.size());

    uint8_t matched_id = 0;
    EXPECT_FALSE(MaybeMatchLiteralTokensExactly(view, kLMUTokens, matched_id));
    EXPECT_EQ(matched_id, 0);
    EXPECT_FALSE(
        MaybeMatchLiteralTokensCaseInsensitively(view, kLMUTokens, matched_id));
    EXPECT_EQ(matched_id, 0);
  }
}

TEST(LiteralTokenTest, LMUTokens) {
  for (const auto& lit_cases : {
           LitCases{kAllLowerStr, {true, kAllLowerId}, {true, kAllLowerId}},
           LitCases{kMixedCaseStr, {true, kMixedCaseId}, {true, kAllLowerId}},
           LitCases{kAllUpperStr, {true, kAllUpperId}, {true, kAllLowerId}},
       }) {
    // Make a copy of the string so ptr equality is known to not happen.
    std::string str(lit_cases.char_ptr);
    StringView view(str);

    // Exact match.
    {
      auto lit_case = lit_cases.exact;
      const uint8_t kNoMatchId = ~lit_case.match_id;
      uint8_t matched_id = kNoMatchId;

      if (lit_case.matches) {
        EXPECT_TRUE(
            MaybeMatchLiteralTokensExactly(view, kLMUTokens, matched_id))
            << "\nview: " << view.ToHexEscapedString();
        EXPECT_EQ(matched_id, lit_case.match_id);
      } else {
        EXPECT_FALSE(
            MaybeMatchLiteralTokensExactly(view, kLMUTokens, matched_id))
            << "\nview: " << view.ToHexEscapedString();
        EXPECT_EQ(matched_id, kNoMatchId);
      }
    }

    // Case-insensitive match.
    {
      auto lit_case = lit_cases.caseless;
      const uint8_t kNoMatchId = ~lit_case.match_id;
      uint8_t matched_id = kNoMatchId;

      if (lit_case.matches) {
        EXPECT_TRUE(MaybeMatchLiteralTokensCaseInsensitively(view, kLMUTokens,
                                                             matched_id))
            << "\nview: " << view.ToHexEscapedString();
        EXPECT_EQ(matched_id, lit_case.match_id);
      } else {
        EXPECT_FALSE(MaybeMatchLiteralTokensCaseInsensitively(view, kLMUTokens,
                                                              matched_id))
            << "\nview: " << view.ToHexEscapedString();
        EXPECT_EQ(matched_id, kNoMatchId);
      }
    }
  }
}

TEST(LiteralTokenTest, UMLTokens) {
  for (const auto& lit_cases : {
           LitCases{kAllLowerStr,
                    {true, kAllLowerId},   // exact
                    {true, kAllUpperId}},  // caseless
           LitCases{kMixedCaseStr,
                    {true, kMixedCaseId},  // exact
                    {true, kAllUpperId}},  // caseless
           LitCases{kAllUpperStr,
                    {true, kAllUpperId},   // exact
                    {true, kAllUpperId}},  // caseless
       }) {
    // Make a copy of the string so ptr equality is known to not happen.
    std::string str(lit_cases.char_ptr);
    StringView view(str);

    // Exact match.
    {
      auto lit_case = lit_cases.exact;
      const uint8_t kNoMatchId = ~lit_case.match_id;
      uint8_t matched_id = kNoMatchId;

      if (lit_case.matches) {
        EXPECT_TRUE(
            MaybeMatchLiteralTokensExactly(view, kUMLTokens, matched_id))
            << "\nview: " << view.ToHexEscapedString();
        EXPECT_EQ(matched_id, lit_case.match_id);
      } else {
        EXPECT_FALSE(
            MaybeMatchLiteralTokensExactly(view, kUMLTokens, matched_id))
            << "\nview: " << view.ToHexEscapedString();
        EXPECT_EQ(matched_id, kNoMatchId);
      }
    }

    // Case-insensitive match.
    {
      auto lit_case = lit_cases.caseless;
      const uint8_t kNoMatchId = ~lit_case.match_id;
      uint8_t matched_id = kNoMatchId;

      if (lit_case.matches) {
        EXPECT_TRUE(MaybeMatchLiteralTokensCaseInsensitively(view, kUMLTokens,
                                                             matched_id))
            << "\nview: " << view.ToHexEscapedString();
        EXPECT_EQ(matched_id, lit_case.match_id);
      } else {
        EXPECT_FALSE(MaybeMatchLiteralTokensCaseInsensitively(view, kUMLTokens,
                                                              matched_id))
            << "\nview: " << view.ToHexEscapedString();
        EXPECT_EQ(matched_id, kNoMatchId);
      }
    }
  }
}

}  // namespace
}  // namespace alpaca
