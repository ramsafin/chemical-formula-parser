#include <gtest/gtest.h>

#include <format>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "cfp/tokenizer.hpp"
#include "cfp/tokenizer_error.hpp"

using token_tuple_t = std::tuple<cfp::TokenKind, std::string, std::optional<uint64_t>>;

// Compare token against an expected (kind, text, value) tuple
void assertTokenEquals(const cfp::Token &token, const token_tuple_t &expected) {
  const auto &[kind, text, value] = expected;
  EXPECT_EQ(token.kind, kind);
  EXPECT_EQ(std::string(token.text), text);
  EXPECT_EQ(token.value, value);
}

// Parameterized test for valid lexemes
class TokenizerValidTest : public ::testing::TestWithParam<std::tuple<std::string, std::vector<token_tuple_t>>> {};

// Ensure the tokenizer produces the exact expected sequence of tokens
TEST_P(TokenizerValidTest, ProducesExpectedTokens) {
  const auto &[input, expected] = GetParam();
  cfp::Tokenizer tokenizer{input};
  std::vector<cfp::Token> out;

  while (true) {
    const cfp::Token tok = tokenizer.peek();
    out.push_back(tok);
    if (tok.kind == cfp::TokenKind::End) {
      break;
    }
    tokenizer.next();
  }

  ASSERT_EQ(out.size(), expected.size());

  for (size_t i = 0; i < expected.size(); ++i) {
    assertTokenEquals(out[i], expected[i]);
  }
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
  ValidCases,
  TokenizerValidTest,
  ::testing::Values(
    std::make_tuple("H",
      std::vector<token_tuple_t>{
        {cfp::TokenKind::Element, "H",   std::nullopt},
        {cfp::TokenKind::End,     "",    std::nullopt}
      }
    ),
    std::make_tuple("Fe",
      std::vector<token_tuple_t>{
        {cfp::TokenKind::Element, "Fe",  std::nullopt},
        {cfp::TokenKind::End,     "",    std::nullopt}
      }
    ),
    std::make_tuple("C12",
      std::vector<token_tuple_t>{
        {cfp::TokenKind::Element, "C",   std::nullopt},
        {cfp::TokenKind::Number,  "12",  12},
        {cfp::TokenKind::End,     "",    std::nullopt}
      }
    ),
    std::make_tuple("Ag12",
      std::vector<token_tuple_t>{
        {cfp::TokenKind::Element, "Ag",  std::nullopt},
        {cfp::TokenKind::Number,  "12",  12},
        {cfp::TokenKind::End,     "",    std::nullopt}
      }
    ),
    std::make_tuple("X1",
      std::vector<token_tuple_t>{
        {cfp::TokenKind::Element, "X",   std::nullopt},
        {cfp::TokenKind::Number,  "1",   1},
        {cfp::TokenKind::End,     "",    std::nullopt}
      }
    ),
    std::make_tuple("2H",
      std::vector<token_tuple_t>{
        {cfp::TokenKind::Number,  "2",   2},
        {cfp::TokenKind::Element, "H",   std::nullopt},
        {cfp::TokenKind::End,     "",    std::nullopt}
      }
    )
  )
);
// clang-format on

// Parameterized test for inputs that must trigger TokenizerError
struct InvalidCase {
  std::string input;
  std::string expected_substr;
};

class TokenizerInvalidTest : public ::testing::TestWithParam<InvalidCase> {};

// For each invalid input, verify a TokenizerError is thrown
TEST_P(TokenizerInvalidTest, ThrowsTokenizerError) {
  const auto &[input, expected_substr] = GetParam();

  try {
    cfp::Tokenizer tokenizer{input};
    tokenizer.next();

    FAIL() << "Expected TokenizerError for input: " << input;
  } catch (const cfp::TokenizerError &e) {
    std::string msg = e.what();

    EXPECT_NE(msg.find(expected_substr), std::string::npos)
        << std::format("Error message '{}' does not contain '{}'", msg, expected_substr);
  } catch (...) {
    FAIL() << "Expected TokenizerError, but got a different exception for input: " << input;
  }
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
  InvalidCases,
  TokenizerInvalidTest,
  ::testing::Values(
    InvalidCase{" ",   "whitespace not allowed"},
    InvalidCase{"H 2", "whitespace not allowed"},
    InvalidCase{"cl",  "unexpected character 'c'"},
    InvalidCase{"H0",  "must be ≥ 1"},
    InvalidCase{"H01", "leading zero"},
    InvalidCase{"H-2", "unexpected character '-'"},
    InvalidCase{"H$",  "unexpected character '$'"},
    InvalidCase{"$H",  "unexpected character '$'"}
  )
);
// clang-format on
