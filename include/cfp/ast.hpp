#pragma once

#include <cstdint>  // uint64_t
#include <memory>  // unique_ptr
#include <string>
#include <string_view>
#include <vector>

namespace cfp {

struct Node {
  virtual ~Node() = default;
};

// Leaf: single element + count
struct ElementNode : Node {
  std::string element;
  uint64_t count;

  ElementNode(std::string_view elem, uint64_t count) : element{elem}, count{count} {}
};

// Interior: group of children with a multiplier
struct GroupNode : Node {
  std::vector<std::unique_ptr<Node>> children;
  uint64_t multiplier = 1;

  explicit GroupNode(uint64_t mult = 1) : multiplier{mult} {}
};

}  // namespace cfp
