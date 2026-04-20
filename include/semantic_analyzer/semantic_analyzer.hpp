#pragma once
#include "parser/ast.hpp"
#include "parser/type.hpp"
#include "symbol_table.hpp"
struct SemanticAnalyzer {
  std::unique_ptr<SymbolTable> st;
  std::optional<TypeNode> curr_func_ret_type;
  SemanticAnalyzer(AST *const root) { visit_program(root); }
  void visit_program(AST *const node);
  void visit_decl(AST *const node);
  void visit_global_decl(AST *const node);
  void visit_id(AST *const node);
  void visit_type(AST *const node);
  void visit_expr(AST *const node);
  void visit_assign_expr(AST *const node);
  void visit_range_expr(AST *const node);
  void visit_logical_or_expr(AST *const node);
  void visit_logical_and_expr(AST *const node);
  void visit_bitwise_or_expr(AST *const node);
  void visit_bitwise_xor_expr(AST *const node);
  void visit_bitwise_and_expr(AST *const node);
  void visit_equality_expr(AST *const node);
  void visit_relational_expr(AST *const node);
  void visit_additive_expr(AST *const node);
  void visit_multiplicative_expr(AST *const node);
  void visit_unary_expr(AST *const node);
  void visit_postfix_expr(AST *const node);
  void visit_primary_expr(AST *const node);
  void visit_const_decl(AST *const node);
  void visit_function_decl(AST *const node);
  void visit_param(AST *const node);
  void visit_block(AST *const node);
  void visit_var_decl(AST *const node);
  void visit_if_stmt(AST *const node);
  void visit_while_stmt(AST *const node);
  void visit_for_stmt(AST *const node);
  void visit_return_stmt(AST *const node);
  void visit_print_stmt(AST *const node);
  void visit_expr_stmt(AST *const node);
  void visit_extern_fn_decl(AST *const node);
};
inline void SemanticAnalyzer::visit_program(AST *const node) {
  for (const auto &child : node->children) {
    visit_decl(child.get());
  }
}
inline void SemanticAnalyzer::visit_decl(AST *const node) {
  switch (node->node) {
  case Node::GLOBAL_DECL:
    visit_global_decl(node);
    break;
  case Node::CONST_DECL:
    visit_const_decl(node);
    break;
  case Node::FUNCTION_DECL:
    visit_function_decl(node);
    break;
  case Node::EXTERN_DECL:
    visit_extern_fn_decl(node);
    break;
  default:
    break;
  }
}
inline void SemanticAnalyzer::visit_global_decl(AST *const node) {
  std::string id = std::get<std::string>(node->children[0]->v);
  Type type_left;
  type_left.is_null = true;
  if (std::holds_alternative<TypeNode>(node->children[1]->v)) {
    type_left.is_null = false;
    type_left.type_node = std::get<TypeNode>(node->children[1]->v);
    switch (type_left.type_node) {
    case TypeNode::VECTOR:
    case TypeNode::MATRIX:
    case TypeNode::TENSOR: {
      type_left.base_type = std::get<TypeNode>(node->children[2]->v);
      for (int i = 3; i < node->children.size(); i++) {
        if (std::holds_alternative<long long>(node->children[i]->v)) {
          type_left.sizes.push_back(std::get<long long>(node->children[i]->v));
        } else {
          break;
        }
      }
    } break;
    default:
      break;
    }
  }
}
