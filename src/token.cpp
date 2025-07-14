#include "cfp/token.hpp"

#include "cfp/token_kind.hpp"  // to_string

namespace cfp {

std::ostream &operator<<(std::ostream &os, Token const &token) {  // NOLINT(*-length)
  os << to_string(token.kind) << "(\"" << token.text << "\")";
  if (token.value) {
    os << "{" << *token.value << "}";
  }
  return os;
}

}  // namespace cfp
