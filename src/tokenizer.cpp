#include "cfp/tokenizer.hpp"

#include <cctype>  // std::isspace, std::isupper, std::islower, std::isdigit
#include <cstdint>  // uint64_t
#include <format>
#include <string>

#include "cfp/tokenizer_error.hpp"

namespace cfp {

Tokenizer::Tokenizer(std::string_view input) : input_{input} {
  if (input.empty()) {
    Token token{.kind = TokenKind::Invalid, .text = ""};
    throw TokenizerError{0, input_, token, "empty input not allowed"};
  }
  nextToken();
}

const Token &Tokenizer::peek() const {
  return currToken_;
}

void Tokenizer::next() {
  nextToken();
}

void Tokenizer::nextToken() {
  if (pos_ >= input_.size()) {
    currToken_ = {.kind = TokenKind::End, .text = {}};
    return;
  }

  const char currChar = input_[pos_];

  if (std::isspace(static_cast<unsigned char>(currChar))) {
    Token token{.kind = TokenKind::Invalid, .text = input_.substr(pos_, 1)};
    throw TokenizerError{pos_, input_, token, "whitespace not allowed"};
  }

  // [A-Z][a-z]*
  if (std::isupper(static_cast<unsigned char>(currChar))) {
    currToken_ = lexElement();
    return;
  }

  // [1-9]?[0-9]*
  if (std::isdigit(static_cast<unsigned char>(currChar))) {
    currToken_ = lexNumber();
    return;
  }

  if (currChar == '(' || currChar == ')' || currChar == '[' || currChar == ']') {
    currToken_ = lexDelimiter(currChar);
    return;
  }

  Token token{.kind = TokenKind::Invalid, .text = input_.substr(pos_, 1)};
  throw TokenizerError{pos_, input_, token, std::format("unexpected character '{}'", currChar)};
}

Token Tokenizer::lexElement() {
  const size_t start = pos_;
  pos_ += 1;

  while (pos_ < input_.size() && std::islower(static_cast<unsigned char>(input_[pos_]))) {
    pos_ += 1;
  }

  return {.kind = TokenKind::Element, .text = input_.substr(start, pos_ - start)};
}

Token Tokenizer::lexNumber() {
  const size_t start = pos_;

  while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) {
    pos_ += 1;
  }

  const auto text = input_.substr(start, pos_ - start);
  Token token{.kind = TokenKind::Number, .text = text};

  const uint64_t value = std::stoull(std::string(text));
  token.value = value;

  if (value == 0) {
    throw TokenizerError{start, input_, token, "invalid count (must be ≥ 1)"};
  }

  if (text.starts_with('0')) {
    throw TokenizerError{start, input_, token, "invalid count (leading zero)"};
  }

  return token;
}

Token Tokenizer::lexDelimiter(char del) {
  Token token;
  switch (del) {
    case '(':
      token = {.kind = TokenKind::LParen, .text = "("};
      break;
    case ')':
      token = {.kind = TokenKind::RParen, .text = ")"};
      break;
    case '[':
      token = {.kind = TokenKind::LBracket, .text = "["};
      break;
    case ']':
      token = {.kind = TokenKind::RBracket, .text = "]"};
      break;
    default:
      token = {.kind = TokenKind::Invalid, .text = input_.substr(pos_, 1)};
  }
  pos_ += 1;
  return token;
}

}  // namespace cfp
