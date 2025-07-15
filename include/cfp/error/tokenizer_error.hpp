#pragma once

#include <cstddef>  // size_t
#include <stdexcept>  // runtime_error
#include <string>
#include <string_view>

#include "cfp/token.hpp"

namespace cfp {

/**
 * @class TokenizerError
 * @brief Exception thrown when the tokenizer encounters an invalid lexeme.
 *
 * Carries the position within the input, a copy of the input string,
 * and the offending Token.
 */
class TokenizerError final : public std::runtime_error {
public:
  /// Zero-based index in the input string where the error occurred.
  size_t offset{0};

  /// Copy of the full input text being tokenized.
  // TODO(Ramil Safin): slice the input (performance issues).
  std::string input;

  /// Token that triggered the error.
  Token token;

  /**
   * @brief Construct a new TokenizerError.
   *
   * The exception’s what() message is formatted as:
   *   "<msg>; token='<tok.text>' (kind=<tok.kind>) at pos <pos> in `<input>`"
   *
   * @param pos   Index in input where the error was detected.
   * @param in    Full input string (copied internally).
   * @param tok   Offending token lexeme.
   * @param msg   Short description of the error condition.
   */
  TokenizerError(size_t pos, std::string_view in,  // NOLINT(*-identifier-length)
                 const Token &tok, std::string_view msg);
};

}  // namespace cfp
