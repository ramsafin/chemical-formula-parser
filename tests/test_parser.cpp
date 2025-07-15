// tests/parser_test.cpp

#include <gtest/gtest.h>

#include <string_view>
#include <unordered_map>

#include "cfp/parser.hpp"

using ExpectedMap = std::unordered_map<std::string, uint64_t>;

// Simple element and sequence tests
class ParserSimpleTest : public ::testing::TestWithParam<std::tuple<std::string_view, ExpectedMap>> {};

TEST_P(ParserSimpleTest, ParsesElementsAndSequences) {
  const auto &[input, expected] = GetParam();

  cfp::Parser parser{input};
  const auto result = parser.parse();

  EXPECT_EQ(result, expected);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
  Simple,
  ParserSimpleTest,
  ::testing::Values(
    std::make_tuple("H",         ExpectedMap{{"H",  1}}),
    std::make_tuple("O2",        ExpectedMap{{"O",  2}}),
    std::make_tuple("HOH",       ExpectedMap{{"H",  2},{"O",  1}}),
    std::make_tuple("Fe2Fe3",    ExpectedMap{{"Fe", 5}}),
    std::make_tuple("C12H22O11", ExpectedMap{{"C", 12},{"H", 22},{"O", 11}})
  )
);
// clang-format on

// Bracketed group tests
class ParserGroupTest : public ::testing::TestWithParam<std::tuple<std::string_view, ExpectedMap>> {};

TEST_P(ParserGroupTest, ParsesBracketedGroups) {
  const auto &[input, expected] = GetParam();

  cfp::Parser parser{input};
  const auto result = parser.parse();

  EXPECT_EQ(result, expected);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
  Groups,
  ParserGroupTest,
  ::testing::Values(
    std::make_tuple("(H)",            ExpectedMap{{"H",  1}}),
    std::make_tuple("((H)2)3",        ExpectedMap{{"H",  6}}),
    std::make_tuple("Mg(OH)2",        ExpectedMap{{"Mg", 1},{"O",  2},{"H",  2}}),
    std::make_tuple("Fe2(SO4)3",      ExpectedMap{{"Fe", 2},{"S",  3},{"O", 12}}),
    std::make_tuple("K[Fe(NO3)2]4",   ExpectedMap{{"K",  1},{"Fe", 4},{"N",  8},{"O", 24}}),
    std::make_tuple("Al[OH]3",        ExpectedMap{{"Al", 1},{"O",  3},{"H",  3}}),
    std::make_tuple("Ca(OH)2H2",      ExpectedMap{{"Ca", 1},{"O",  2},{"H",  4}}),
    std::make_tuple("Na[Cl]",         ExpectedMap{{"Na", 1},{"Cl", 1}})
  )
);
// clang-format on

// Ligand groups tests
class ParserLigandTest : public ::testing::TestWithParam<std::tuple<std::string_view, ExpectedMap>> {};

TEST_P(ParserLigandTest, ParsesLigandNotation) {
  const auto &[input, expected] = GetParam();

  cfp::Parser parser{input};
  const auto result = parser.parse();

  EXPECT_EQ(result, expected);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
  Ligands,
  ParserLigandTest,
  ::testing::Values(
    std::make_tuple("CuSO4*5H2O",    ExpectedMap{{"Cu",  1},{"S", 1},{"O",  9},{"H", 10}}),
    std::make_tuple("2H2O*3NH3",     ExpectedMap{{"H",  13},{"O", 2},{"N",  3}}),
    std::make_tuple("Fe2(SO4)3*H2O", ExpectedMap{{"Fe",  2},{"S", 3},{"O", 13},{"H",  2}})
  )
);
// clang-format on

// Invalid formula tests

class ParserInvalidTest : public ::testing::TestWithParam<std::string_view> {};

TEST_P(ParserInvalidTest, ThrowsOnInvalidInput) {
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
  Invalid,
  ParserInvalidTest,
  ::testing::Values(
    "",              // empty
    "()",            // empty group
    "(]",            // mismatched brackets
    "(",             // missing closer
    "H2O)",          // stray closer
    "[H2O",          // missing closer
    "H2O]",          // stray closer
    "(H2O]2",        // mismatched
    "*H2O",          // leading '*'
    "H2O*",          // trailing '*'
    "H2O**H2O",      // empty between
    "Fe2(SO4)0",     // zero multiplier
    "Fe2(SO4)03",    // leading zero group multiplier
    "Mg(O0H)2",      // zero count inside
    "H2O*0H2O",      // zero prefix multiplier
    "H2O*05H2O",     // leading zero prefix
    "H2O*1.5H2O",    // fractional invalid
    "K[Fe(NO3)2]4a"  // trailing invalid char
  )
);
// clang-format on
