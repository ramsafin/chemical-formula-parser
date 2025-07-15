#include "cfp/parser.hpp"

#include <format>

#include "cfp/error/parser_error.hpp"

namespace cfp {

Parser::Parser(std::string_view input) : tokenizer_(input) {}

std::unordered_map<std::string, uint64_t> Parser::parse() {
  auto root = parseAST();

  std::unordered_map<std::string, uint64_t> counts;
  root->evaluate(counts, /*mult=*/1);

  return counts;
}

std::unique_ptr<GroupNode> Parser::parseAST() {
  // top-level group (may contain multiple units separated by Star)
  auto root = std::make_unique<GroupNode>(/*mult=*/1);

  while (true) {
    // optional prefix multiplier for unit
    uint64_t unit_mult = 1;

    if (const auto token = tokenizer_.peek(); token.kind == TokenKind::Number) {
      unit_mult = *token.value;
      tokenizer_.next();
    }

    // parse one formula unit (stops at Star or End)
    auto unit = parseFormula(TokenKind::Star);

    if (unit->children.empty()) {
      throw ParserError{tokenizer_.peek(), "empty unit between '*'"};
    }

    unit->multiplier = unit_mult;
    root->children.emplace_back(std::move(unit));

    // if there is a Star, consume it and handle the next unit
    if (const auto token = tokenizer_.peek(); token.kind == TokenKind::Star) {
      tokenizer_.next();
      continue;
    }

    // no units left
    break;
  }

  // handle invalid trailing tokens
  if (const auto token = tokenizer_.peek(); token.kind != TokenKind::End) {
    throw ParserError{token, std::format("unexpected token '{}' after unit", token.text)};
  }

  return root;
}

std::unique_ptr<GroupNode> Parser::parseFormula(TokenKind closing) {
  auto group = std::make_unique<GroupNode>(/*mult=*/1);

  while (true) {
    const auto token = tokenizer_.peek();

    if (token.kind == closing || token.kind == TokenKind::End) {
      break;  // reached the end of formula
    }

    group->children.push_back(parseGroup());
  }

  return group;
}

std::unique_ptr<Node> Parser::parseGroup() {
  const auto token = tokenizer_.peek();

  // element + optional count
  if (token.kind == TokenKind::Element) {
    tokenizer_.next();

    uint64_t count = 1;

    if (const auto next_token = tokenizer_.peek(); next_token.kind == TokenKind::Number) {
      count = *next_token.value;
      tokenizer_.next();
    }

    return std::make_unique<ElementNode>(token.text, count);
  }

  // group: opening paren '(' or bracket '['
  if (token.kind == TokenKind::LParen || token.kind == TokenKind::LBracket) {
    tokenizer_.next();

    const bool is_paren = (token.kind == TokenKind::LParen);
    const auto matching_closer = is_paren ? TokenKind::RParen : TokenKind::RBracket;

    // parse inner formula up to matching ')' or ']'
    auto inner_group = parseFormula(matching_closer);

    if (inner_group->children.empty()) {
      throw ParserError{token, "empty group not allowed"};
    }

    // expect matching closing token
    if (const auto closer_token = tokenizer_.peek(); closer_token.kind != matching_closer) {
      // clang-format off
      throw ParserError{
        closer_token, is_paren ?
        "unmatched '(' — expected ')'" : "unmatched '[' — expected ']'"
      };
      // clang-format on
    }

    tokenizer_.next();

    // optional group multiplier
    uint64_t mult = 1;

    if (const auto next_token = tokenizer_.peek(); next_token.kind == TokenKind::Number) {
      mult = *next_token.value;
      tokenizer_.next();
    }

    inner_group->multiplier = mult;
    return inner_group;
  }

  // anything else is an error
  throw ParserError{token, "expected element or group"};
}

}  // namespace cfp
