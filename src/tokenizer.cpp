#include "cfp/tokenizer.hpp"

#include <cctype>  // std::isspace, std::isupper, std::islower, std::isdigit
#include <cstdint>  // uint64_t
#include <format>
#include <string>

#include "cfp/tokenizer_error.hpp"

namespace cfp {

Tokenizer::Tokenizer(std::string_view input) : input_{input} {
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

  const char curr_char = input_[pos_];

  if (std::isspace(static_cast<unsigned char>(curr_char))) {
    Token token{.kind = TokenKind::Invalid, .text = input_.substr(pos_, 1)};
    throw TokenizerError{pos_, input_, token, "whitespace not allowed"};
  }

  // [A-Z][a-z]*
  if (std::isupper(static_cast<unsigned char>(curr_char))) {
    currToken_ = lexElement();
    return;
  }

  // [1-9]?[0-9]*
  if (std::isdigit(static_cast<unsigned char>(curr_char))) {
    currToken_ = lexNumber();
    return;
  }

  // any other single character is invalid
  Token token{.kind = TokenKind::Invalid, .text = input_.substr(pos_, 1)};
  throw TokenizerError{pos_, input_, token, std::format("unexpected character '{}'", curr_char)};
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

}  // namespace cfp
