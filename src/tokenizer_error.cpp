#include "cfp/tokenizer_error.hpp"

#include <format>

#include "cfp/token_kind.hpp"  // to_string

namespace cfp {

TokenizerError::TokenizerError(size_t pos, std::string_view in,  // NOLINT(*-length)
                               const Token &tok, std::string_view msg) :
    // clang-format off
    std::runtime_error{std::format(
      "{}; token='{}' (kind={}) at pos {} in `{}`",
      msg, tok.text, to_string(tok.kind), pos, in
    )},
    // clang-format on
    position{pos}, input{in}, token{tok} {}

}  // namespace cfp
