// tests/tokenizer_test.cpp

#include <gtest/gtest.h>

#include <format>
#include <optional>
#include <string_view>
#include <tuple>
#include <vector>

#include "cfp/error/tokenizer_error.hpp"
#include "cfp/tokenizer.hpp"

using token_tuple_t = std::tuple<cfp::TokenKind, std::string_view, std::optional<uint64_t>>;

static void assertTokenEquals(const cfp::Token &token, const token_tuple_t &exp) {
  const auto &[kind, text, value] = exp;
  EXPECT_EQ(token.kind, kind);
  EXPECT_EQ(token.text, text);
  EXPECT_EQ(token.value, value);
}

// Constructor and peek/next behavior
TEST(TokenizerTest, ConstructorThrowsOnEmptyInput) {
  EXPECT_THROW({ cfp::Tokenizer tokenizer{""}; }, cfp::TokenizerError);
}

TEST(TokenizerTest, PeekDoesNotAdvance) {
  cfp::Tokenizer tokenizer{"H2"};
  const auto token1 = tokenizer.peek();
  const auto token2 = tokenizer.peek();

  EXPECT_EQ(token1.kind, token2.kind);
  EXPECT_EQ(token1.text, token2.text);
}

TEST(TokenizerTest, NextAdvancesToken) {
  cfp::Tokenizer tokenizer{"H2"};

  const auto first = tokenizer.peek();
  EXPECT_EQ(first.kind, cfp::TokenKind::Element);

  tokenizer.next();

  const auto second = tokenizer.peek();
  EXPECT_EQ(second.kind, cfp::TokenKind::Number);
}

// Simple element/number sequences
class TokenizerSimpleTest : public ::testing::TestWithParam<std::tuple<std::string_view, std::vector<token_tuple_t>>> {
};

TEST_P(TokenizerSimpleTest, ProducesExpectedSequence) {
  const auto &[input, expected] = GetParam();
  cfp::Tokenizer tokenizer{input};
  std::vector<cfp::Token> out;

  while (true) {
    auto token = tokenizer.peek();
    out.push_back(token);
    if (token.kind == cfp::TokenKind::End) {
      break;
    }
    tokenizer.next();
  }

  ASSERT_EQ(out.size(), expected.size());

  for (size_t idx = 0; idx < out.size(); idx++) {
    assertTokenEquals(out[idx], expected[idx]);
  }
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(SimpleSequences, TokenizerSimpleTest,
  ::testing::Values(
    std::make_tuple("H",
      std::vector<token_tuple_t>{
        {cfp::TokenKind::Element, "H", std::nullopt},
        {cfp::TokenKind::End,     "",  std::nullopt}
      }),
    std::make_tuple("Fe2",
      std::vector<token_tuple_t>{
        {cfp::TokenKind::Element, "Fe", std::nullopt},
        {cfp::TokenKind::Number,  "2",  2},
        {cfp::TokenKind::End,     "",   std::nullopt}
      }),
    std::make_tuple("C12H",
      std::vector<token_tuple_t>{
        {cfp::TokenKind::Element, "C",  std::nullopt},
        {cfp::TokenKind::Number,  "12", 12},
        {cfp::TokenKind::Element, "H",  std::nullopt},
        {cfp::TokenKind::End,     "",   std::nullopt}
      }),
    std::make_tuple("C3H7OH",
      std::vector<token_tuple_t>{
        {cfp::TokenKind::Element, "C",  std::nullopt},
        {cfp::TokenKind::Number,  "3",  3},
        {cfp::TokenKind::Element, "H",  std::nullopt},
        {cfp::TokenKind::Number,  "7",  7},
        {cfp::TokenKind::Element, "O",  std::nullopt},
        {cfp::TokenKind::Element, "H",  std::nullopt},
        {cfp::TokenKind::End,     "",   std::nullopt}
      })
  )
);
// clang-format on

// Delimiters
class TokenizerDelimiterTest : public ::testing::TestWithParam<std::tuple<std::string_view, cfp::TokenKind>> {};

TEST_P(TokenizerDelimiterTest, RecognizesSingleCharTokens) {
  const auto &[del, kind] = GetParam();

  cfp::Tokenizer tokenizer{del};
  const auto token = tokenizer.peek();

  EXPECT_EQ(token.kind, kind);
  EXPECT_EQ(token.text, del);

  EXPECT_NO_THROW(tokenizer.next());
  EXPECT_EQ(tokenizer.peek().kind, cfp::TokenKind::End);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(Delimiters, TokenizerDelimiterTest,
  ::testing::Values(
    std::make_tuple("(", cfp::TokenKind::LParen),
    std::make_tuple(")", cfp::TokenKind::RParen),
    std::make_tuple("[", cfp::TokenKind::LBracket),
    std::make_tuple("]", cfp::TokenKind::RBracket),
    std::make_tuple("*", cfp::TokenKind::Star)
));
// clang-format on

// Invalid input cases
struct InvalidCase {
  std::string_view input, expect_sub;
};

class TokenizerInvalidTest : public ::testing::TestWithParam<InvalidCase> {};

TEST_P(TokenizerInvalidTest, ThrowsWithExpectedMessage) {
  const auto &[input, expect_sub] = GetParam();

  try {
    cfp::Tokenizer tokenizer{input};
    tokenizer.next();
    FAIL() << "Expected TokenizerError for \"" << input << "\"";
  } catch (const cfp::TokenizerError &err) {
    std::string_view what = err.what();
    EXPECT_NE(what.find(expect_sub), std::string::npos) << std::format("Message '{}' missing '{}'", what, expect_sub);
  } catch (...) {
    FAIL() << "Wrong exception type for \"" << input << "\"";
  }
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InvalidInputs, TokenizerInvalidTest,
  ::testing::Values(
    InvalidCase{"",    "empty input"},
    InvalidCase{" ",   "whitespace"},
    InvalidCase{"H 2", "whitespace"},
    InvalidCase{"cl",  "unexpected character 'c'"},
    InvalidCase{"H0",  "non-positive"},
    InvalidCase{"H01", "leading zero"},
    InvalidCase{"H-2", "unexpected character '-'"},
    InvalidCase{"H$",  "unexpected character '$'"},
    InvalidCase{"$H",  "unexpected character '$'"}
  )
);
// clang-format on
