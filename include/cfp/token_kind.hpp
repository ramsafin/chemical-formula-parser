#pragma once

#include <cstdint>  // uint8_t
#include <string_view>

namespace cfp {

/**
 * @enum TokenKind
 * @brief Lexical category of a token produced by the tokenizer.
 */
enum class TokenKind : uint8_t {
  /// Chemical element symbol (e.g. "H", "Fe").
  Element,

  /// Positive integer count (e.g. "1", "12").
  Number,

  /// Left parenthesis '(' opening a group.
  LParen,

  /// Right parenthesis ')' closing a group.
  RParen,

  /// Left bracket '[' opening a group.
  LBracket,

  /// Right bracket ']' closing a group.
  RBracket,

  /// Asterisk '*' used as a ligand/hydrate separator.
  Star,

  /// EOF marker (no more tokens).
  End,

  /// Invalid or unrecognized lexeme.
  Invalid
};

/**
 * @brief Convert a TokenKind to a human-readable name.
 *
 * @param kind  TokenKind to stringify.
 * @return      String literals representation.
 */
constexpr std::string_view to_string(TokenKind kind) noexcept {
  // clang-format off
  switch (kind) {
    case TokenKind::Element:  return "Element";
    case TokenKind::Number:   return "Number";
    case TokenKind::LParen:   return "LParen";
    case TokenKind::RParen:   return "RParen";
    case TokenKind::LBracket: return "LBracket";
    case TokenKind::RBracket: return "RBracket";
    case TokenKind::Star:     return "Star";
    case TokenKind::End:      return "End";
    case TokenKind::Invalid:  [[fallthrough]];
    default:                  return "Invalid";
  }
  // clang-format on
}

}  // namespace cfp
