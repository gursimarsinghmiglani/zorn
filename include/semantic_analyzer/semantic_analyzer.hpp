#pragma once
#include "parser/ast.hpp"
#include "parser/type.hpp"
#include "symbol_table.hpp"
#include <iostream>
struct SemanticAnalyzer {
  std::unique_ptr<SymbolTable> st;
  std::optional<TypeNode> curr_func_ret_type;
  SemanticAnalyzer(AST *const root) { visit_program(root); }
  void error(AST *const root) {

  void visit_program(AST *const node);
  void visit_decl(AST *const node);
  void visit_var_decl(AST *const node);
  static Type type_unify(const Type& type_left, const Type& type_right);
  static Type base_type_node_unify(TypeNode left, TypeNode right);
  static Type container_type_unify(const Type& left, const Type& right);
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
  case Node::VAR_DECL:
    visit_var_decl(node);
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
inline void SemanticAnalyzer::visit_var_decl(AST *const node) {
  std::string id = std::get<std::string>(node->children[0]->v);
  Type type;
  if (node->children[1]->node == Node::TYPE) {
    Type type_left = node->children[1]->type;
    if (node->children.size() > 2) {
      visit_expr(node->children[2].get());
      Type type_right = node->children[2]->type;
      type = type_unify(type_left, type_right);
    } else {
      type = type_left;
    }
  } else {
    visit_expr(node->children[1].get());
    type = node->children[1]->type;
  }
  node->type = type;
  SymbolInfo sym_info(type, false);
  bool success = st->declare(id, sym_info);
  if (!successs)
}
inline void SemanticAnalyzer::visit_const_decl(AST *node) {
  SymbolInfo sym_info;
  std::string id = std::get<std::string>(node->children[0]->v);
  Type type;
  if (node->children[1]->node == Node::TYPE) {
    Type type_left = node->children[1]->type;
    visit_expr(node->children[2].get());
    Type type_right = node->children[2]->type;
    type = type_unify(type_left, type_right);
  } else {
    visit_expr(node->children[1].get());
    type = node->children[1]
}
inline Type SemanticAnalyzer::type_unify(const Type& type_left, const Type& type_right) {
  switch (type_left.type_node) {
    case TypeNode::INT:
    case TypeNode::FLOAT:
    case TypeNode::BOOL:
      return base_type_node_unify(type_left.type_node, type_right.type_node);
    case TypeNode::VECTOR:
    case TypeNode::MATRIX:
    case TypeNode::TENSOR:
      return container_type_unify(type_left, type_right);
    default:
      return Type::error();
  }
}
inline Type SemanticAnalyzer::base_type_node_unify(TypeNode left, TypeNode right) {
  switch (left) {
    case TypeNode::INT:
    case TypeNode::FLOAT:
    case TypeNode::BOOL:
      switch (right) {
        case TypeNode::INT:
        case TypeNode::FLOAT:
        case TypeNode::BOOL:
          return Type::from_type_node(left);
        default:
          return Type::error();
      }
    default:
      return Type::error();
  }
}
inline Type SemanticAnalyzer::container_type_unify(const Type& left, const Type& right) {
  switch (right.type_node) {
    case TypeNode::INT:
    case TypeNode::BOOL:
    case TypeNode::FLOAT:
    case TypeNode::VOID:
      return Type::error();
    default:
      if (left.sizes != right.sizes) {
        return Type::error();
      }
      TypeNode base_type = base_type_node_unify(left.base_type.value(), right.base_type.value()).type_node;
      Type t = Type::from_type_node(left.type_node);
      t.base_type = base_type;
      t.sizes = left.sizes;
      return t;
  }
}
