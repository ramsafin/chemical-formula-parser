#pragma once

#include <cstdint>  // uint64_t
#include <string>
#include <string_view>
#include <unordered_map>

#include "cfp/tokenizer.hpp"

namespace cfp {

/**
 * @class Parser
 * @brief Parses simple chemical formula consisting of exactly one element
 *        and an optional positive integer count (Part 1).
 *
 * This parser relies on Tokenizer for lexing.
 * It consumes exactly Element [ Number ] and then expects end-of-input.
 *
 * Example valid inputs:
 *  - "Fe"   => { "Fe": 1 }
 *  - "O2"   => { "O" : 2 }
 *  - "Ag12" => { "Ag": 12 }
 */
class Parser {
public:
  /**
   * @brief Constructs parser for the given input string
   *
   * @param input  chemical-formula-like string to parse
   * @throws TokenizerError if lexing error occurs at position 0.
   */
  explicit Parser(std::string_view input);

  /**
   * @brief Parses input as exactly one element plus optional count.
   *
   * Grammar:
   *   <formula> := <element> [ <number> ]
   *   <element> := UppercaseLetter { LowercaseLetter }
   *   <number>  := Digit { Digit } (positive integer, no leading zeros)
   *
   * @return dictionary containing exactly one entry: {element: count}.
   *
   * @throws ParserError     if the first token is not an element, or if there
   *                         are extra tokens after the optional number.
   * @throws TokenizerError  if lexing error occurs while retrieving tokens
   */
  std::unordered_map<std::string, uint64_t> parse();

private:
  // lexer for breaking input into tokens
  Tokenizer tokenizer_;
};

}  // namespace cfp
