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
  // empty formula check
  if (const auto token = tokenizer_.peek(); token.kind == TokenKind::End) {
    throw ParserError{token, "empty formula"};
  }

  // top-level group (may contain multiple units separated by Star)
  auto root = std::make_unique<GroupNode>(/*mult=*/1);

  // one or more units separated by '*'
  while (true) {
    // optional prefix multiplier
    uint64_t unit_mult = 1;

    if (const auto token = tokenizer_.peek(); token.kind == TokenKind::Number) {
      unit_mult = *token.value;
      tokenizer_.next();
    }

    if (const auto token = tokenizer_.peek(); token.kind == TokenKind::Star || token.kind == TokenKind::End) {
      throw ParserError{token, std::format("expected formula after multiplier ({})", unit_mult)};
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

  // no trailing tokens allowed
  if (const auto token = tokenizer_.peek(); token.kind != TokenKind::End) {
    throw ParserError{token, std::format("unexpected token '{}' after unit", token.text)};
  }

  return root;
}

std::unique_ptr<GroupNode> Parser::parseFormula(TokenKind closing) {
  auto group = std::make_unique<GroupNode>(/*mult=*/1);

  while (true) {
    const auto token = tokenizer_.peek();

    // clang-format off
    // handle mismatched brackets/paren
    if (
      (closing == TokenKind::RParen || closing == TokenKind::RBracket) &&
      (token.kind == TokenKind::RParen || token.kind == TokenKind::RBracket) && token.kind != closing) {
      throw ParserError{
        token,
        std::format(
          "unmatched '{}' - expected '{}'",
          token.text, closing == TokenKind::RParen ? ")" : "]"
        )
      };
    }
    // clang-format on

    if (token.kind == closing || token.kind == TokenKind::End) {
      break;  // reached the end of formula
    }

    group->children.emplace_back(parseGroup());
  }

  return group;
}

std::unique_ptr<Node> Parser::parseGroup() {
  const auto token = tokenizer_.peek();

  // invalid '*' inside the group
  if (token.kind == TokenKind::Star) {
    throw ParserError{token, "unexpected '*' inside group"};
  }

  // invalid closing brackets/paren inside the group
  if (token.kind == TokenKind::RParen || token.kind == TokenKind::RBracket) {
    throw ParserError{token, std::format("unmatched '{}'", token.text)};
  }

  // Element [Number]
  if (token.kind == TokenKind::Element) {
    tokenizer_.next();

    uint64_t count = 1;

    if (const auto next_token = tokenizer_.peek(); next_token.kind == TokenKind::Number) {
      count = *next_token.value;
      tokenizer_.next();
    }

    return std::make_unique<ElementNode>(token.text, count);
  }

  // '(' formula ')' [Number] or '[' formula ']' [Number]
  if (token.kind == TokenKind::LParen || token.kind == TokenKind::LBracket) {
    const bool is_paren = (token.kind == TokenKind::LParen);
    const auto matching_closer = is_paren ? TokenKind::RParen : TokenKind::RBracket;

    tokenizer_.next();

    // parse inner formula up to matching bracket/paren
    auto subgroup = parseFormula(matching_closer);

    if (const auto closer = tokenizer_.peek(); closer.kind != matching_closer) {
      throw ParserError{closer, is_paren ? "unmatched '(' - expected ')'" : "unmatched '[' - expected ']'"};
    }

    if (subgroup->children.empty()) {
      throw ParserError{token, "empty group not allowed"};
    }

    tokenizer_.next();

    // optional multiplier
    uint64_t group_mult = 1;

    if (const auto next_token = tokenizer_.peek(); next_token.kind == TokenKind::Number) {
      group_mult = *next_token.value;
      tokenizer_.next();
    }

    subgroup->multiplier = group_mult;
    return subgroup;
  }

  // anything else is an error
  throw ParserError{token, "expected element or group"};
}

}  // namespace cfp
