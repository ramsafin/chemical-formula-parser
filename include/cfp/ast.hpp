#pragma once

#include <cstdint>  // uint64_t
#include <memory>  // unique_ptr
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace cfp {

using ElementCountDict = std::unordered_map<std::string, uint64_t>;

/**
 * @brief Abstract base for AST nodes.
 */
struct Node {
  virtual ~Node() = default;

  /**
   * @brief Evaluate a node into an element-count map.
   * @param out   Map to accumulate into.
   * @param mult  Multiplier from parent groups.
   */
  virtual void evaluate(ElementCountDict &out, uint64_t mult) const = 0;
};

/**
 * @brief Leaf: a single element symbol with count.
 */
struct ElementNode final : Node {
  std::string symbol;
  uint64_t count{1};

  explicit ElementNode(std::string_view sym, uint64_t count) : symbol{sym}, count{count} {}

  void evaluate(ElementCountDict &out, uint64_t mult) const override {
    out[symbol] += count * mult;
  }
};

/**
 * @brief Interior node: a group of children, with a multiplier.
 *
 * Example: "(SO4)3" produces a GroupNode with
 *   children = { ElementNode{"S", 1}, ElementNode{"O", 4} }
 *   multiplier = 3
 */
struct GroupNode final : Node {
  std::vector<std::unique_ptr<Node>> children;
  uint64_t multiplier{1};

  explicit GroupNode(uint64_t mult = 1) : multiplier{mult} {}

  void evaluate(ElementCountDict &out, uint64_t mult) const override {
    const uint64_t next_mult = mult * multiplier;

    for (const auto &child : children) {
      child->evaluate(out, next_mult);
    }
  }
};

}  // namespace cfp
