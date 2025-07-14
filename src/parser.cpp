#include "cfp/parser.hpp"

#include "cfp/parser_error.hpp"

namespace cfp {

Parser::Parser(std::string_view input) : tokenizer_(input) {}

std::unordered_map<std::string, uint64_t> Parser::parse() {
  std::unordered_map<std::string, uint64_t> dict{};

  while (true) {
    Token token = tokenizer_.peek();

    if (token.kind == TokenKind::End) {
      if (dict.empty()) {
        throw ParserError{token, "expected at least one element"};
      }
      break;
    }

    if (token.kind != TokenKind::Element) {
      throw ParserError{token, "expected element or end"};
    }

    std::string element{token.text};
    tokenizer_.next();

    uint64_t count = 1;
    token = tokenizer_.peek();

    if (token.kind == TokenKind::Number) {
      tokenizer_.next();
      count = *token.value;
    }

    // Accumulate
    dict[element] += count;
  }

  return dict;
}

}  // namespace cfp
