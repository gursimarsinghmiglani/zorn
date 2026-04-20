#pragma once
#include "parser/type_node.hpp"
#include <optional>
#include <vector>
struct Type {
  bool is_null;
  TypeNode type_node;
  std::optional<TypeNode> base_type;
  std::vector<int> sizes;
};
