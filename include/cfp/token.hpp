#pragma once

#include <cstdint>  // uint64_t
#include <iosfwd>  // ostream
#include <optional>
#include <string_view>

#include "cfp/token_kind.hpp"

namespace cfp {

/**
 * @struct Token
 * @brief Lexical token produced by the tokenizer.
 */
struct Token {
  /// Category (Element, Number, etc.).
  TokenKind kind{TokenKind::Invalid};

  /// Exact slice of the input this token represents.
  std::string_view text;

  /// For Number tokens only, the parsed positive integer.
  std::optional<uint64_t> value{std::nullopt};
};

/**
 * @brief Output stream operator for tokens.
 *
 * Renders as, e.g. `Element("Fe")` or `Number("12"){12}`.
 *
 * @param os     Destination stream
 * @param token  The token to print
 * @return       Reference to @a os
 */
std::ostream &operator<<(std::ostream &os, const Token &token);  // NOLINT(*-identifier-length)

}  // namespace cfp
