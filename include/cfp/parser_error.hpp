#pragma once

#include <stdexcept>  // runtime_error
#include <string_view>

#include "cfp/token.hpp"

namespace cfp {

/**
 * @class ParserError
 * @brief Exception thrown when parser encounters grammar violation
 */
class ParserError : public std::runtime_error {
public:
  // token that caused parse failure
  Token token;

  /**
   * @brief Constructs ParserError
   *
   * Formats a message of the form:
   *   "Parse error: <msg>; token='<token.text>' (kind=<TokenKind>)"
   *
   * @param tok token at which parsing failed
   * @param msg short error description
   */
  ParserError(const Token &tok, std::string_view msg);
};

}  // namespace cfp
