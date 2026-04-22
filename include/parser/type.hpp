#pragma once
#include "parser/type_node.hpp"
#include <optional>
#include <vector>
struct Type {
  bool is_null;
  TypeNode type_node;
  std::optional<TypeNode> base_type;
  std::vector<int> sizes;
  static Type error() {
    Type err;
    err.is_null = true;
    return err;
  }
  static Type from_type_node(TypeNode type_node) {
    Type t;
    t.is_null = false;
    t.type_node = type_node;
    return t;
  }
};
