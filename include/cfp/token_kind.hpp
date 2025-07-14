#pragma once

#include <cstdint>  // uint8_t
#include <string_view>

namespace cfp {

/**
 * @enum TokenKind
 * @brief Lexical category of token produced by tokenizer
 *
 * - Element:  element symbol token (e.g. "H", "Fe")
 * - Number:   positive integer count token (e.g. "1", "12")
 * - LParen:   left parenthesis (opens a group)
 * - RParen:   right parenthesis (closes a group)
 * - LBracket: left bracket (opens a group)
 * - RBracket  right bracket (closes a group)
 * - End:      end-of-input marker (indicates no more tokens)
 * - Invalid:  lexeme that failed validation (used in error-handling)
 */
enum class TokenKind : uint8_t {
  Element,  // chemical element symbol
  Number,  // numeric multiplier
  LParen,  // '('
  RParen,  // ')'
  LBracket,  // '['
  RBracket,  // ']'
  End,  // end of input marker
  Invalid  // invalid or unrecognized lexeme
};

/**
 * @brief Converts TokenKind to human-readable name
 *
 * @param kind  TokenKind to stringify
 * @return      human-readable name of token
 */
std::string_view to_string(TokenKind kind);

}  // namespace cfp
