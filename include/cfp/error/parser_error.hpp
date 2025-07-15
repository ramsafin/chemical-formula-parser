#pragma once

#include <stdexcept>  // runtime_error
#include <string_view>

#include "cfp/token.hpp"

namespace cfp {

/**
 * @class ParserError
 * @brief Exception thrown when the parser encounters a grammar violation.
 *
 * Carries the offending token to inspect what failed.
 */
class ParserError final : public std::runtime_error {
public:
  /// Token that caused the parse failure.
  Token token;

  /**
   * @brief Construct a new ParserError.
   *
   * The exception’s what() string will be:
   *   "Parse error: <message>; token='<text>' (kind=<kind>)"
   *
   * @param tok   Token at which parsing failed.
   * @param msg   Short description of the error.
   */
  ParserError(const Token &tok, std::string_view msg);
};

}  // namespace cfp
