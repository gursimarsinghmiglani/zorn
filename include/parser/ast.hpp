#pragma once
#include "binary_op_node.hpp"
#include "lexer/lexeme.hpp"
#include "node.hpp"
#include "postfix_op_node.hpp"
#include "print_node.hpp"
#include "type.hpp"
#include "unary_op_node.hpp"
#include <optional>
#include <string>
#include <variant>
#include <vector>
struct AST {
  Node node;
  std::variant<long long, double, bool, std::string, BinaryOpNode,
               PostfixOpNode, TypeNode, UnaryOpNode, PrintNode>
      v;
  std::vector<std::unique_ptr<AST>> children;
  Type type;
  std::optional<Lexeme> lexeme;
};
