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
 * @brief Parses chemical formula into element counts using an AST
 *
 * Supports nested bracket groups with multipliers, e.g.:
 *  - Fe2(SO4)3  => {Fe: 2, S: 3, O: 12}
 *  - K[Fe(NO3)2]4 => {K: 1, Fe: 4, N: 8, O: 24}
 *
 * Parsing is done in two phases:
 * 1. Build AST of ElementNode and GroupNode via recursive-descent
 * 2. Walk AST to multiply and accumulate counts into flat map
 */
class Parser {
public:
  /**
   * @brief Constructs parser for the given input string
   *
   * @param input  chemical-formula-like string to parse
   * @throws TokenizerError if lexing error occurs at position 0
   */
  explicit Parser(std::string_view input);

  /**
   * @brief Parse input and retun {element, count} map
   *
   * @return dictionary containing element-count pairs
   *
   * @throws ParserError     if grammar is violated
   * @throws TokenizerError  if lexing error occurs while retrieving tokens
   */
  std::unordered_map<std::string, uint64_t> parse();

private:
  // lexer for breaking input into tokens
  Tokenizer tokenizer_;

  /**
   * @brief Builds AST tree (a GroupNode with multiplier = 1)
   *
   * Parses the whole formula into nested GroupNode/ElementNode tree structure
   */
  std::unique_ptr<GroupNode> parseAST();

  /**
   * @brief Parses sequence of groups until a closing token or EOF
   *
   * Grammar:
   *   <formula> ::= (<group>)*
   *
   * Stops when peek() yields `closing` or `TokenKind::End`.
   *
   * @param closing  TokenKind that signals the end of sub-formula, e.g. TokenKind::RParen or TokenKind::RBracket.
   *                 Pass TokenKind::Invalid at top level to stop only at EOF.
   *
   * @return GroupNode containing all parsed children, with default multiplier = 1.
   */
  std::unique_ptr<GroupNode> parseFormula(TokenKind closing = TokenKind::Invalid);

  /**
   * @brief Parses single group:
   *  - ElementNode: element with optional count, or
   *  - GroupNode: a parenthesized/bracketed sub-formula with optional multiplier
   *
   * Grammar:
   *   <group> ::= <element> [ <number> ]
   *             | '(' <formula> ')' [ <number> ]
   *             | '[' <formula> ']' [ <number> ]
   *
   * @return Node pointing to either an ElementNode or GroupNode.
   */
  std::unique_ptr<Node> parseGroup();
};

}  // namespace cfp
