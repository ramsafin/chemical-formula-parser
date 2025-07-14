#include <gtest/gtest.h>

#include <cstdint>
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
  ValidCases,
  ParserValidTest,
  ::testing::Values(
    // simple parentheses
    std::make_tuple("Mg(OH)2",      ExpectedMap{{"Mg",1}, {"O",2}, {"H",2}}),
    std::make_tuple("(H)",          ExpectedMap{{"H",1}}),
    std::make_tuple("((H)2)3",      ExpectedMap{{"H",6}}),
    // sulfate example
    std::make_tuple("Fe2(SO4)3",    ExpectedMap{{"Fe",2}, {"S",3}, {"O",12}}),
    // nested brackets and parens
    std::make_tuple("K[Fe(NO3)2]4", ExpectedMap{{"K",1}, {"Fe",4}, {"N",8}, {"O",24}}),
    // square‐bracket only
    std::make_tuple("Al[OH]3",      ExpectedMap{{"Al",1}, {"O",3}, {"H",3}}),
    // combination of multiple groups
    std::make_tuple("Ca(OH)2H2",    ExpectedMap{{"Ca",1}, {"O",2}, {"H",4}}),
    // no multiplier = default 1
    std::make_tuple("Na[Cl]",       ExpectedMap{{"Na",1}, {"Cl",1}})
  )
);
// clang-format on

// Parameterized test for formulas that should trigger errors
class ParserInvalidTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ParserInvalidTest, ThrowsOnInvalidFormulas) {
  const auto &input = GetParam();

  EXPECT_THROW(
      {
        cfp::Parser parser{input};
        parser.parse();
      },
      std::exception);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
  InvalidCases,
  ParserInvalidTest,
  ::testing::Values(
    "",               // empty
    "()",             // empty group
    "(]"              // mismtached brackets
    "(",              // missing closing paren
    "H2O)",           // stray closing paren
    "[H2O",           // missing closing bracket
    "H2O]",           // stray closing bracket
    "(H2O]2",         // mismatched brackets
    "Fe2(SO4)0",      // zero multiplier on group
    "Fe2(SO4)03",     // leading zero on group multiplier
    "Mg(O0H)2",       // zero count inside group
    "K[Fe(NO3)2]4a",  // trailing invalid char
    "Ca(OH)2)H2",     // extra closing paren in middle
    "Al[OH3",         // missing closing bracket after number
    "Al(OH",          // missing closing paren
    "Al(OH)1.5"       // fractional multiplier invalid
  )
);
// clang-format on
