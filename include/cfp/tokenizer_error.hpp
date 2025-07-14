#pragma once

#include <cstddef>  // size_t
#include <stdexcept>  // runtime_error
#include <string>
#include <string_view>

#include "cfp/token.hpp"

namespace cfp {

/**
 * @class TokenizerError
 * @brief Exception thrown when tokenizer encounters invalid lexeme
 */
class TokenizerError : public std::runtime_error {
public:
  // index in input string where error occurred
  size_t position{0};

  // copy of input string being tokenized
  std::string input;

  // token that triggered error
  Token token;

  /**
   * @brief Constructs TokenizerError
   *
   * Formats a message of the form:
   *   "<msg>; token='<tok.text>' (kind=<tok.kind>) at pos <pos> in `<input>`"
   *
   * @param pos   index in input where error was detected
   * @param in    full input text being tokenized
   * @param tok   token object corresponding to invalid lexeme
   * @param msg   short description of error
   */
  TokenizerError(size_t pos, std::string_view in, const Token &tok,  // NOLINT(*-length)
                 std::string_view msg);
};

}  // namespace cfp
