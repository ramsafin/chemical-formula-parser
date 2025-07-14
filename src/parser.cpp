#include "cfp/parser.hpp"

#include <functional>  // function

#include "cfp/parser_error.hpp"

namespace cfp {

Parser::Parser(std::string_view input) : tokenizer_(input) {}

std::unordered_map<std::string, uint64_t> Parser::parse() {
  std::unordered_map<std::string, uint64_t> dict{};

  auto root = parseAST();

  // walk the AST, accumulating counts with respect to group multipliers
  std::function<void(Node *, uint64_t)> visit = [&](Node *node, uint64_t mult) -> void {
    // Leaf: element * count * current multiplier
    if (auto *elem_node = dynamic_cast<ElementNode *>(node); elem_node != nullptr) {
      dict[elem_node->element] += elem_node->count * mult;
    } else if (auto *group_node = dynamic_cast<GroupNode *>(node); group_node != nullptr) {
      // Interior: multiply down and recurse on children
      uint64_t combined = mult * group_node->multiplier;
      for (auto &child : group_node->children) {
        visit(child.get(), combined);
      }
    }
  };

  visit(root.get(), 1);
  return dict;
}

std::unique_ptr<GroupNode> Parser::parseAST() {
  auto root = parseFormula(/* closing = */ TokenKind::Invalid);

  if (auto token = tokenizer_.peek(); token.kind != TokenKind::End) {
    throw ParserError{token, "extra token after formula"};
  }

  return root;
}

std::unique_ptr<GroupNode> Parser::parseFormula(TokenKind closing) {
  auto group = std::make_unique<GroupNode>(/* mult = */ 1);

  while (true) {
    auto token = tokenizer_.peek();

    if (token.kind == closing || token.kind == TokenKind::End) {
      break;  // end of formula
    }

    group->children.push_back(parseGroup());
  }

  return group;
}

std::unique_ptr<Node> Parser::parseGroup() {
  const auto token = tokenizer_.peek();

  // 1. Element
  if (token.kind == TokenKind::Element) {
    tokenizer_.next();

    std::string elem(token.text);
    uint64_t count = 1;

    if (auto nextToken = tokenizer_.peek(); nextToken.kind == TokenKind::Number) {
      count = *nextToken.value;
      tokenizer_.next();
    }

    return std::make_unique<ElementNode>(std::move(elem), count);
  }

  // 2. Parenthesis / bracket
  if (token.kind == TokenKind::LParen || token.kind == TokenKind::LBracket) {
    // consume the openning '(' or '['
    tokenizer_.next();

    const bool isParen = (token.kind == TokenKind::LParen);
    const auto closing = isParen ? TokenKind::RParen : TokenKind::RBracket;

    // parse sub‐formula up to matching ')' or ']'
    auto subgroup = parseFormula(closing);

    if (subgroup->children.empty()) {
      throw ParserError{token, "empty group not allowed"};
    }

    // expect matching closer
    auto closer = tokenizer_.peek();

    if (closer.kind != closing) {
      // clang-format off
      throw ParserError{
        closer, isParen ?
        "unmatched '(' — expected ')'" : "unmatched '[' — expected ']'"
      };
      // clang-format on
    }

    // consume the closing ')' or ']'
    tokenizer_.next();

    // optional multiplier
    uint64_t mult = 1;

    if (const auto nextToken = tokenizer_.peek(); nextToken.kind == TokenKind::Number) {
      mult = *nextToken.value;
      tokenizer_.next();
    }

    subgroup->multiplier = mult;
    return subgroup;
  }

  // 3. Anything else is a error
  throw ParserError{token, "expected element or group"};
}

}  // namespace cfp
