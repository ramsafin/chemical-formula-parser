#pragma once

#include <cstddef>  // size_t
#include <string_view>

#include "cfp/token.hpp"

namespace cfp {

/**
 * @class Tokenizer
 * @brief Lexical analyzer for chemical-formula tokens.
 *
 * Recognizes the following tokens:
 *   - Element:  uppercase letter followed by zero or more lowercase letters
 *   - Number:   a sequence of digits (positive integer, no leading zeros)
 *   - LParen:   '('
 *   - RParen:   ')'
 *   - LBracket: '['
 *   - RBracket: ']'
 *   - Star:     '*' (ligand separator)
 *   - End:      EOF marker
 *
 * Throws TokenizerError on any invalid lexeme.
 */
class Tokenizer {
public:
  /**
   * @brief Construct and consume the first token.
   * @param input  Formula string to tokenize (must not be empty).
   * @throws TokenizerError if input is empty or the first lexeme is invalid.
   */
  explicit Tokenizer(std::string_view input);

  /**
   * @brief Peek at the current token without consuming it.
   * @return Const reference to the current Token.
   */
  [[nodiscard]] const Token &peek() const noexcept;

  /**
   * @brief Consume the current token and advance to the next one.
   * @throws TokenizerError if the next lexeme is invalid.
   */
  void next();

private:
  /// Entire input being tokenized.
  std::string_view input_;

  // Current index in input (zero-based offset).
  size_t offset_{0};

  // Most recently lexed token.
  Token curr_token_;

  /**
   * @brief Lex an Element token at the current position.
   * @return A Token of kind Element with its text.
   */
  Token lexElementToken();

  /**
   * @brief Lex a Number token at the current position.
   * @return A Token of kind Number with its text and numeric value.
   * @throws TokenizerError on zero value or leading zero.
   */
  Token lexNumberToken();

  /**
   * @brief Lex a single‐character token: '(', ')', '[', ']', or '*'.
   * @param del The delimiter character.
   * @return A Token of the corresponding kind.
   */
  Token lexSingleCharToken(char del);
};

}  // namespace cfp
