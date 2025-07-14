#include "cfp/token_kind.hpp"

namespace cfp {

std::string_view to_string(TokenKind kind) {
  switch (kind) {
    case TokenKind::Element:
      return "Element";
    case TokenKind::Number:
      return "Number";
    case TokenKind::LParen:
      return "LParen";
    case TokenKind::RParen:
      return "RParen";
    case TokenKind::LBracket:
      return "LBracket";
    case TokenKind::RBracket:
      return "RBracket";
    case TokenKind::End:
      return "End";
    case TokenKind::Invalid:
      [[fallthrough]];
    default:
      return "Invalid";
  }
}

}  // namespace cfp
