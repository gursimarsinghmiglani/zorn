#pragma once
#include "parser/type_node.hpp"
#include <vector>
struct Type {
  bool is_null;
  TypeNode type_node;
  TypeNode base_type;
  std::vector<int64_t> sizes;
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
