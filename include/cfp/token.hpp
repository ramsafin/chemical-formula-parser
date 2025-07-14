#pragma once

#include <cstdint>  // uint64_t
#include <optional>
#include <ostream>
#include <string_view>

#include "cfp/token_kind.hpp"

namespace cfp {

/**
 * @struct Token
 * @brief Single lexical token produced by tokenizer
 */
struct Token {
  // category of token
  TokenKind kind;

  // raw lexeme text
  std::string_view text;

  // numeric value (only for Number tokens)
  std::optional<uint64_t> value{std::nullopt};
};

/**
 * @brief Helper to output tokens into streams
 *
 * Prints token kind and its text, plus “{value}” if present.
 *
 * @param os     output stream
 * @param token  token to print
 */
std::ostream &operator<<(std::ostream &os, Token const &token);  // NOLINT(*-length)

}  // namespace cfp
