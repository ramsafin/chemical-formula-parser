#pragma once

#include <cstdint>  // uint64_t
#include <string>
#include <string_view>
#include <unordered_map>

#include "cfp/tokenizer.hpp"

namespace cfp {

/**
 * @class Parser
 * @brief Parses chemical formula consisting of one or more Element[Number]? tokens.
 *
 * Examples:
 *  - "Fe2O3" → { "Fe": 2, "O": 3 }
 *  - "H2SO4" → { "H": 2, "S": 1, "O": 4 }
 *  - "HOH"   → { "H": 2, "O": 1 }
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
   * @brief Parse input as sequence of Element[Number]? tokens
   *
   * @return dictionary containing element-count pairs
   *
   * @throws ParserError     if unexpected token appears
   * @throws TokenizerError  if lexing error occurs while retrieving tokens
   */
  std::unordered_map<std::string, uint64_t> parse();

private:
  // lexer for breaking input into tokens
  Tokenizer tokenizer_;
};

}  // namespace cfp
