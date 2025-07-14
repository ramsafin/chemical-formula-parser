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
    std::make_tuple("H",         ExpectedMap{{"H",   1}}),
    std::make_tuple("O2",        ExpectedMap{{"O",   2}}),
    std::make_tuple("H2O",       ExpectedMap{{"H",   2}, {"O", 1}}),
    std::make_tuple("HOH",       ExpectedMap{{"H",   2}, {"O", 1}}),
    std::make_tuple("Fe2O3",     ExpectedMap{{"Fe",  2}, {"O", 3}}),
    std::make_tuple("NaCl",      ExpectedMap{{"Na",  1}, {"Cl",1}}),
    std::make_tuple("C12H22O11", ExpectedMap{{"C",  12}, {"H",22}, {"O",11}}),
    std::make_tuple("HHH",       ExpectedMap{{"H",   3}}),
    std::make_tuple("Fe2Fe3",    ExpectedMap{{"Fe",  5}}),
    std::make_tuple("CH",        ExpectedMap{{"C",   1}, {"H", 1}})
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
    ""      ,  // empty string
    "2H"    ,  // leading digit
    "cl"    ,  // lowercase element
    "H0"    ,  // zero count
    "H01"   ,  // leading zero
    "H-2"   ,  // invalid character '-'
    "H2O$"  ,  // invalid character '$'
    " H2O"  ,  // leading whitespace
    "H2O "  ,  // trailing whitespace
    "(HO)"  ,  // unsupported parentheses
    "HO)"   ,  // stray closing parenthesis
    "*HO"   ,  // unsupported ligand syntax
    "Fe2 O3"   // whitespace in formula
  )
);
// clang-format on
