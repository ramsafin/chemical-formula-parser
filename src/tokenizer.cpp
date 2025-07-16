#include "cfp/tokenizer.hpp"

#include <cassert>
#include <cctype>  // std::isspace, std::isupper, std::islower, std::isdigit
#include <cstdint>  // uint64_t
#include <format>
#include <string>  // stoull

#include "cfp/error/tokenizer_error.hpp"

namespace cfp {

Tokenizer::Tokenizer(std::string_view input) : input_{input} {
  if (input.empty()) {
    throw TokenizerError{0, input_, {.kind = TokenKind::Invalid, .text = {}}, "empty input not allowed"};
  }
  // consume the first token
  next();
}

const Token &Tokenizer::peek() const noexcept {
  return curr_token_;
}

void Tokenizer::next() {
  if (offset_ >= input_.size()) {
    curr_token_ = {.kind = TokenKind::End, .text = {}};
    return;
  }

  const char curr_char = input_[offset_];

  if (std::isspace(static_cast<unsigned char>(curr_char))) {
    // clang-format off
    throw TokenizerError{
      offset_, input_,
      {.kind = TokenKind::Invalid, .text = input_.substr(offset_, 1)},
      "whitespace not allowed"
    };
    // clang-format on
  }

  if (std::isupper(static_cast<unsigned char>(curr_char))) {
    curr_token_ = lexElementToken();
    return;
  }

  if (std::isdigit(static_cast<unsigned char>(curr_char))) {
    curr_token_ = lexNumberToken();
    return;
  }

  if (curr_char == '(' || curr_char == ')' || curr_char == '[' || curr_char == ']' || curr_char == '*') {
    curr_token_ = lexSingleCharToken(curr_char);
    return;
  }

  // clang-format off
  throw TokenizerError{
    offset_, input_,
    {.kind = TokenKind::Invalid, .text = input_.substr(offset_, 1)},
    std::format("unexpected character '{}'", curr_char)
  };
  // clang-format on
}

Token Tokenizer::lexElementToken() {
  assert(std::isupper(static_cast<unsigned char>(input_[offset_])));

  const size_t start = offset_;
  offset_ += 1;

  while (offset_ < input_.size() && std::islower(static_cast<unsigned char>(input_[offset_]))) {
    offset_ += 1;
  }

  const auto text = input_.substr(start, offset_ - start);
  return {.kind = TokenKind::Element, .text = text};
}

Token Tokenizer::lexNumberToken() {
  assert(std::isdigit(static_cast<unsigned char>(input_[offset_])));

  const size_t start = offset_;
  offset_ += 1;

  while (offset_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[offset_]))) {
    offset_ += 1;
  }

  const auto text = input_.substr(start, offset_ - start);
  const uint64_t value = std::stoull(std::string{text});

  const Token token{.kind = TokenKind::Number, .text = text, .value = value};

  if (value == 0) {
    throw TokenizerError{start, input_, token, "invalid number (non-positive integer)"};
  }

  if (text.starts_with('0')) {
    throw TokenizerError{start, input_, token, "invalid number (leading zero)"};
  }

  return token;
}

Token Tokenizer::lexSingleCharToken(char del) {
  assert(del == '(' || del == ')' || del == '[' || del == ']' || del == '*');

  TokenKind kind;

  // clang-format off
  switch (del) {
    case '(': kind = TokenKind::LParen;
      break;
    case ')': kind = TokenKind::RParen;
      break;
    case '[': kind = TokenKind::LBracket;
      break;
    case ']': kind = TokenKind::RBracket;
      break;
    case '*': kind = TokenKind::Star;
      break;
    default:  kind = TokenKind::Invalid;
      break;
  }
  // clang-format on

  const auto text = input_.substr(offset_, 1);
  offset_ += 1;

  return Token{.kind = kind, .text = text};
}

}  // namespace cfp
