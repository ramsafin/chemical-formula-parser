#include <gtest/gtest.h>

#include <string>
#include <tuple>
#include <unordered_map>

#include "cfp/parser.hpp"

using ExpectedMap = std::unordered_map<std::string, uint64_t>;

// Parameterized test for valid single-element formulas
class ParserValidTest : public ::testing::TestWithParam<std::tuple<std::string, ExpectedMap>> {};

TEST_P(ParserValidTest, ParsesSingleElementCorrectly) {
  const auto &[input, expected] = GetParam();
  cfp::Parser parser{input};
  auto result = parser.parse();
  EXPECT_EQ(result, expected);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
  ValidInputs,
  ParserValidTest,
  ::testing::Values(
    std::make_tuple("H",    ExpectedMap{{"H",   1}}),
    std::make_tuple("Fe",   ExpectedMap{{"Fe",  1}}),
    std::make_tuple("O2",   ExpectedMap{{"O",   2}}),
    std::make_tuple("Ag12", ExpectedMap{{"Ag", 12}}),
    std::make_tuple("O123", ExpectedMap{{"O", 123}})
  )
);
// clang-format on

// Parameterized test for formulas that should trigger errors
class ParserInvalidTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ParserInvalidTest, ThrowsOnBadInput) {
  const auto &input = GetParam();
  EXPECT_THROW(
      {
        cfp::Parser parser{input};
        parser.parse();
      },
      std::runtime_error);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
  InvalidInputs,
  ParserInvalidTest,
  ::testing::Values(
    ""   ,  // empty string
    "2H" ,  // leading digit
    "cl" ,  // lowercase element
    "N0" ,  // zero count
    "H01",  // leading zero
    "H2O",  // extra token after element
    " H" ,  // leading whitespace
    "H 2" , // whitespace in the middle
    "H " ,  // trailing whitespace
    "X-5"   // invalid character '-'
  )
);
// clang-format on
