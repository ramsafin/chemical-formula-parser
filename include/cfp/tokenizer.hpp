#pragma once

#include <cstddef>  // size_t
#include <string_view>

#include "cfp/token.hpp"

namespace cfp {

/**
 * @class Tokenizer
 * @brief Lexical analyzer for simple chemical-formula tokens (Part 1)
 *
 * Splits the input string into a sequence of Tokens:
 *  - Element: uppercase letter followed by zero or more lowercase letters
 *  - Number:  sequence of digits (validated to be ≥1, no leading zeros)
 *  - End:     end-of-input marker
 *  - Invalid: single-character lexemes that violate rules
 */
class Tokenizer {
public:
  /**
   * @brief Constructs Tokenizer and consumes the first token
   * @param input formula string to tokenize)
   * @throws TokenizerError on any invalid lexeme at position 0
   */
  explicit Tokenizer(std::string_view input);

  /**
   * @brief Peeks at the current token without consuming it
   * @return const reference to the current token
   */
  [[nodiscard]] const Token &peek() const;

  /**
   * @brief Consumes the current token, advances to the next one
   * @throws TokenizerError if the next lexeme is invalid
   */
  void next();

private:
  // full input being tokenized
  std::string_view input_;

  // current character index in input
  size_t pos_{0};

  // the most recently lexed token
  Token currToken_;

  /**
   * @brief Lex the next token from input
   *
   * Recognizes Element and Number tokens
   * Throws TokenizerError on whitespace or invalid character
   */
  void nextToken();

  /**
   * @brief Lex Element token starting at the current position
   * @return Element token
   */
  Token lexElement();

  /**
   * @brief Lex Number token starting at the current position
   *
   * Validates that the parsed integer is ≥ 1 and has no leading zeros
   *
   * @return Number token with value set
   * @throws TokenizerError on zero value or leading-zero syntax
   */
  Token lexNumber();
};

}  // namespace cfp
