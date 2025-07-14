#include "cfp/parser.hpp"

#include <cstdint>  // uint64_t

#include "cfp/parser_error.hpp"

namespace cfp {

Parser::Parser(std::string_view input) : tokenizer_(input) {}

std::unordered_map<std::string, uint64_t> Parser::parse() {
  // expect Element token
  Token token = tokenizer_.peek();
  if (token.kind != TokenKind::Element) {
    throw ParserError{token, "expected element at start"};
  }

  // consume the element
  std::string element{token.text};
  tokenizer_.next();

  // Optional count (default = 1)
  uint64_t count = 1;
  token = tokenizer_.peek();

  if (token.kind == TokenKind::Number) {
    tokenizer_.next();
    count = *token.value;
  }

  // no trailing tokens allowed
  token = tokenizer_.peek();

  if (token.kind != TokenKind::End) {
    throw ParserError{token, "unexpected token after element"};
  }

  return {{std::move(element), count}};
}

}  // namespace cfp
