#pragma once

#include <cstdint>  // uint64_t
#include <memory>  // unique_ptr
#include <string>
#include <string_view>
#include <unordered_map>

#include "cfp/ast.hpp"
#include "cfp/tokenizer.hpp"

namespace cfp {

/**
 * @class Parser
 * @brief Parses a chemical formula into element counts.
 *
 * Supports:
 *  - single elements & multipliers:                     Fe2        => {Fe: 2}
 *  - multiple elements:                                 H2O        => {H: 2, O: 1}
 *  - nested () and [] with multipliers                  Fe2(SO4)3  => {Fe: 2, S: 3, O: 12}
 *  - ligand groups (*) with optional prefix multipliers CuSO4*5H2O => {Cu: 1, S: 1, O: 9, H: 10}
 */
class Parser {
public:
  /**
   * @brief Create a parser for the given formula.
   * @param input  Non-empty formula string (no whitespace).
   * @throws TokenizerError on any lex error in the first token.
   */
  explicit Parser(std::string_view input);

  /**
   * @brief Fully parse and evaluate the formula.
   * @return A map of element symbol → total count.
   * @throws ParserError    on grammar errors (mismatches, empties, etc.)
   * @throws TokenizerError on mid-parse lex errors.
   */
  std::unordered_map<std::string, uint64_t> parse();

private:
  /// Lexer for breaking input into tokens.
  Tokenizer tokenizer_;

  /**
   * @brief Build AST tree of units separated by *.
   *
   * Parses the formula into nested GroupNode/ElementNode tree structure.
   */
  std::unique_ptr<GroupNode> parseAST();

  /**
   * @brief Parse a (sub-)formula: zero or more groups up to closing token.
   * @param closing  Token that ends this sub-formula (RParen, RBracket, Star).
   * @return A GroupNode whose children are all the subgroups.
   */
  std::unique_ptr<GroupNode> parseFormula(TokenKind closing = TokenKind::Invalid);

  /**
   * @brief Parse a single “group”:
   *  - ElementNode (symbol + optional number), or
   *  - GroupNode (parenthesized/bracketed formula + optional multiplier)
   *
   * @return ElementNode or GroupNode
   * @throws ParserError if neither element nor paren/bracket is found
   */
  std::unique_ptr<Node> parseGroup();
};

}  // namespace cfp
