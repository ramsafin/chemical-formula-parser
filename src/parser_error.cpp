#include "cfp/parser_error.hpp"

#include <format>

#include "cfp/token_kind.hpp"  // to_string

namespace cfp {

ParserError::ParserError(const Token &tok, std::string_view msg) :
    // clang-format off
    std::runtime_error{std::format(
      "Parse error: {}; token='{}' (kind={})",
      msg, tok.text, to_string(tok.kind)
    )},
    // clang-format on

    token(tok) {}

}  // namespace cfp
