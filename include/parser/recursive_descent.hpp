#pragma once
#include "ast.hpp"
#include <iostream>
struct Parser {
  std::vector<Lexeme> &lexemes;
  size_t pos;
  Parser(std::vector<Lexeme> &lexemes) : lexemes(lexemes), pos(0) {}
  const Lexeme &peek() const { return lexemes[pos]; }
  void advance() { pos++; }
  Token tok() const { return peek().tok; }
  [[noreturn]] void error_function() const {
    std::cerr << "Parsing error in line " << peek().line_number
              << " at position " << pos << "\n";
    std::exit(1);
  }
  bool check(Token t) const { return tok() == t; }
  bool match(Token t) {
    if (tok() == t) {
      advance();
      return true;
    }
    return false;
  }
  void consume(Token t) {
    if (tok() == t) {
      advance();
      return;
    }
    error_function();
  }
  bool is_eof() const { return tok() == Token::TOK_EOF; }
  std::unique_ptr<AST> parse_program();
  std::unique_ptr<AST> parse_global_decl();
  std::unique_ptr<AST> parse_id();
  std::unique_ptr<AST> parse_type();
  std::unique_ptr<AST> parse_base_type();
  std::unique_ptr<AST> parse_int_lit();
  std::unique_ptr<AST> parse_expr();
  std::unique_ptr<AST> parse_assign_expr();
  std::unique_ptr<AST> parse_range_expr();
  std::unique_ptr<AST> parse_logical_or_expr();
  std::unique_ptr<AST> parse_logical_and_expr();
  std::unique_ptr<AST> parse_bitwise_or_expr();
  std::unique_ptr<AST> parse_bitwise_xor_expr();
  std::unique_ptr<AST> parse_bitwise_and_expr();
  std::unique_ptr<AST> parse_equality_expr();
  std::unique_ptr<AST> parse_relational_expr();
  std::unique_ptr<AST> parse_additive_expr();
  std::unique_ptr<AST> parse_multiplicative_expr();
  std::unique_ptr<AST> parse_unary_expr();
  std::unique_ptr<AST> parse_postfix_expr();
  void fill_expr_list(std::unique_ptr<AST> &ast);
  std::unique_ptr<AST> parse_primary_expr();
  std::unique_ptr<AST> parse_const_decl();
  std::unique_ptr<AST> parse_function_decl();
  void fill_param_list(std::unique_ptr<AST> &ast);
  std::unique_ptr<AST> parse_param();
  std::unique_ptr<AST> parse_block();
  std::unique_ptr<AST> parse_var_decl();
  std::unique_ptr<AST> parse_if_stmt();
  std::unique_ptr<AST> parse_while_stmt();
  std::unique_ptr<AST> parse_for_stmt();
  std::unique_ptr<AST> parse_return_stmt();
  std::unique_ptr<AST> parse_print_stmt();
  std::unique_ptr<AST> parse_expr_stmt();
  std::unique_ptr<AST> parse_extern_fn_decl();
};
inline std::unique_ptr<AST> Parser::parse_program() {
  auto ast = std::make_unique<AST>();
  ast->node = Node::PROGRAM;
  while (!is_eof()) {
    if (check(Token::TOK_GLOBAL)) {
      ast->children.push_back(parse_global_decl());
    } else if (check(Token::TOK_CONST)) {
      ast->children.push_back(parse_const_decl());
    } else if (check(Token::TOK_FN)) {
      ast->children.push_back(parse_function_decl());
    } else {
      error_function();
    }
  }
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_global_decl() {
  consume(Token::TOK_GLOBAL);
  auto ast = std::make_unique<AST>();
  ast->node = Node::GLOBAL_DECL;
  ast->children.push_back(parse_id());
  if (match(Token::TOK_COLON)) {
    ast->children.push_back(parse_type());
    if (match(Token::TOK_ASSIGN)) {
      ast->children.push_back(parse_expr());
    }
  } else if (match(Token::TOK_ASSIGN)) {
    ast->children.push_back(parse_expr());
  } else
    error_function();
  consume(Token::TOK_SEMI);
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_id() {
  auto &t = peek();
  consume(Token::TOK_ID);
  auto ast = std::make_unique<AST>();
  ast->node = Node::ID;
  ast->v = t.s;
  ast->lexeme = t;
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_type() {
  auto ast = std::make_unique<AST>();
  ast->node = Node::TYPE;
  if (match(Token::TOK_VECTOR)) {
    ast->v = TypeNode::VECTOR;
    consume(Token::TOK_LESS);
    ast->children.push_back(parse_base_type());
    consume(Token::TOK_COMMA);
    if (!check(Token::TOK_INT_LIT)) {
      error_function();
    }
    ast->children.push_back(parse_int_lit());
    consume(Token::TOK_GREATER);
  } else if (match(Token::TOK_MATRIX)) {
    ast->v = TypeNode::MATRIX;
    consume(Token::TOK_LESS);
    ast->children.push_back(parse_base_type());
    consume(Token::TOK_COMMA);
    ast->children.push_back(parse_int_lit());
    consume(Token::TOK_COMMA);
    ast->children.push_back(parse_int_lit());
    consume(Token::TOK_GREATER);
  } else if (match(Token::TOK_TENSOR)) {
    ast->v = TypeNode::TENSOR;
    consume(Token::TOK_LESS);
    ast->children.push_back(parse_base_type());
    consume(Token::TOK_COMMA);
    ast->children.push_back(parse_int_lit());
    while (match(Token::TOK_COMMA)) {
      ast->children.push_back(parse_int_lit());
    }
    consume(Token::TOK_GREATER);
  } else {
    ast = parse_base_type();
  }
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_base_type() {
  auto ast = std::make_unique<AST>();
  ast->node = Node::TYPE;
  if (match(Token::TOK_INT)) {
    ast->v = TypeNode::INT;
  } else if (match(Token::TOK_FLOAT)) {
    ast->v = TypeNode::FLOAT;
  } else if (match(Token::TOK_BOOL)) {
    ast->v = TypeNode::BOOL;
  } else {
    error_function();
  }
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_int_lit() {
  if (!check(Token::TOK_INT_LIT)) {
    error_function();
  }
  auto ast = std::make_unique<AST>();
  ast->node = Node::INT_LIT;
  ast->v = std::stoll(peek().s);
  advance();
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_expr() { return parse_assign_expr(); }
inline std::unique_ptr<AST> Parser::parse_assign_expr() {
  auto left = parse_range_expr();
  if (match(Token::TOK_ASSIGN)) {
    auto right = parse_assign_expr();
    auto ast = std::make_unique<AST>();
    ast->node = Node::ASSIGN;
    ast->children.push_back(std::move(left));
    ast->children.push_back(std::move(right));
    return ast;
  }
  return left;
}
inline std::unique_ptr<AST> Parser::parse_range_expr() {
  if (match(Token::TOK_RANGE)) {
    auto ast = std::make_unique<AST>();
    ast->node = Node::RANGE;
    consume(Token::TOK_LPAREN);
    ast->children.push_back(parse_expr());
    consume(Token::TOK_COMMA);
    ast->children.push_back(parse_expr());
    if (match(Token::TOK_COMMA)) {
      ast->children.push_back(parse_expr());
    }
    consume(Token::TOK_RPAREN);
    return ast;
  }
  return parse_logical_or_expr();
}
inline std::unique_ptr<AST> Parser::parse_logical_or_expr() {
  auto left = parse_logical_and_expr();
  while (match(Token::TOK_OR)) {
    auto right = parse_logical_and_expr();
    auto ast = std::make_unique<AST>();
    ast->node = Node::BINARY_OP;
    ast->v = BinaryOpNode::OR;
    ast->children.push_back(std::move(left));
    ast->children.push_back(std::move(right));
    left = std::move(ast);
  }
  return left;
}
inline std::unique_ptr<AST> Parser::parse_logical_and_expr() {
  auto left = parse_bitwise_or_expr();
  while (match(Token::TOK_AND)) {
    auto right = parse_bitwise_or_expr();
    auto ast = std::make_unique<AST>();
    ast->node = Node::BINARY_OP;
    ast->v = BinaryOpNode::AND;
    ast->children.push_back(std::move(left));
    ast->children.push_back(std::move(right));
    left = std::move(ast);
  }
  return left;
}
inline std::unique_ptr<AST> Parser::parse_bitwise_or_expr() {
  auto left = parse_bitwise_xor_expr();
  while (match(Token::TOK_BITWISE_OR)) {
    auto right = parse_bitwise_xor_expr();
    auto ast = std::make_unique<AST>();
    ast->node = Node::BINARY_OP;
    ast->v = BinaryOpNode::BITWISE_OR;
    ast->children.push_back(std::move(left));
    ast->children.push_back(std::move(right));
    left = std::move(ast);
  }
  return left;
}
inline std::unique_ptr<AST> Parser::parse_bitwise_xor_expr() {
  auto left = parse_bitwise_and_expr();
  while (match(Token::TOK_BITWISE_XOR)) {
    auto right = parse_bitwise_and_expr();
    auto ast = std::make_unique<AST>();
    ast->node = Node::BINARY_OP;
    ast->v = BinaryOpNode::BITWISE_XOR;
    ast->children.push_back(std::move(left));
    ast->children.push_back(std::move(right));
    left = std::move(ast);
  }
  return left;
}
inline std::unique_ptr<AST> Parser::parse_bitwise_and_expr() {
  auto left = parse_equality_expr();
  while (match(Token::TOK_BITWISE_AND)) {
    auto right = parse_equality_expr();
    auto ast = std::make_unique<AST>();
    ast->node = Node::BINARY_OP;
    ast->v = BinaryOpNode::BITWISE_AND;
    ast->children.push_back(std::move(left));
    ast->children.push_back(std::move(right));
    left = std::move(ast);
  }
  return left;
}
inline std::unique_ptr<AST> Parser::parse_equality_expr() {
  auto left = parse_relational_expr();
  while (check(Token::TOK_EQ) || check(Token::TOK_NEQ)) {
    auto ast = std::make_unique<AST>();
    if (match(Token::TOK_EQ)) {
      ast->v = BinaryOpNode::EQ;
    } else {
      ast->v = BinaryOpNode::NEQ;
    }
    auto right = parse_relational_expr();
    ast->node = Node::BINARY_OP;
    ast->children.push_back(std::move(left));
    ast->children.push_back(std::move(right));
    left = std::move(ast);
  }
  return left;
}
inline std::unique_ptr<AST> Parser::parse_relational_expr() {
  auto left = parse_additive_expr();
  while (1) {
    auto ast = std::make_unique<AST>();
    if (match(Token::TOK_LESS)) {
      ast->v = BinaryOpNode::LESS;
    } else if (match(Token::TOK_LEQ)) {
      ast->v = BinaryOpNode::LEQ;
    } else if (match(Token::TOK_GREATER)) {
      ast->v = BinaryOpNode::GREATER;
    } else if (match(Token::TOK_GEQ)) {
      ast->v = BinaryOpNode::GEQ;
    } else {
      break;
    }
    auto right = parse_additive_expr();
    ast->node = Node::BINARY_OP;
    ast->children.push_back(std::move(left));
    ast->children.push_back(std::move(right));
    left = std::move(ast);
  }
  return left;
}
inline std::unique_ptr<AST> Parser::parse_additive_expr() {
  auto left = parse_multiplicative_expr();
  while (check(Token::TOK_PLUS) || check(Token::TOK_MINUS)) {
    auto ast = std::make_unique<AST>();
    if (match(Token::TOK_PLUS)) {
      ast->v = BinaryOpNode::PLUS;
    } else {
      ast->v = BinaryOpNode::MINUS;
    }
    auto right = parse_multiplicative_expr();
    ast->node = Node::BINARY_OP;
    ast->children.push_back(std::move(left));
    ast->children.push_back(std::move(right));
    left = std::move(ast);
  }
  return left;
}
inline std::unique_ptr<AST> Parser::parse_multiplicative_expr() {
  auto left = parse_unary_expr();
  while (check(Token::TOK_MUL) || check(Token::TOK_DIV) ||
         check(Token::TOK_DOT_MUL) || check(Token::TOK_DOT_DIV)) {
    auto ast = std::make_unique<AST>();
    if (match(Token::TOK_MUL)) {
      ast->v = BinaryOpNode::MUL;
    } else if (match(Token::TOK_DIV)) {
      ast->v = BinaryOpNode::DIV;
    } else if (match(Token::TOK_DOT_MUL)) {
      ast->v = BinaryOpNode::DOT_MUL;
    } else {
      ast->v = BinaryOpNode::DOT_DIV;
    }
    auto right = parse_unary_expr();
    ast->node = Node::BINARY_OP;
    ast->children.push_back(std::move(left));
    ast->children.push_back(std::move(right));
    left = std::move(ast);
  }
  return left;
}
inline std::unique_ptr<AST> Parser::parse_unary_expr() {
  auto ast = std::make_unique<AST>();
  if (match(Token::TOK_MINUS)) {
    ast->node = Node::UNARY_OP;
    ast->v = UnaryOpNode::MINUS;
    ast->children.push_back(parse_unary_expr());
    return ast;
  }
  if (match(Token::TOK_NOT)) {
    ast->node = Node::UNARY_OP;
    ast->v = UnaryOpNode::NOT;
    ast->children.push_back(parse_unary_expr());
    return ast;
  }
  if (match(Token::TOK_BITWISE_NOT)) {
    ast->node = Node::UNARY_OP;
    ast->v = UnaryOpNode::BITWISE_NOT;
    ast->children.push_back(parse_unary_expr());
    return ast;
  }
  return parse_postfix_expr();
}
inline std::unique_ptr<AST> Parser::parse_postfix_expr() {
  auto left = parse_primary_expr();
  while (1) {
    auto ast = std::make_unique<AST>();
    auto right = std::make_unique<AST>();
    if (match(Token::TOK_LBRACKET)) {
      ast->v = PostfixOpNode::INDEX;
      fill_expr_list(right);
      consume(Token::TOK_RBRACKET);
    } else if (match(Token::TOK_LPAREN)) {
      ast->v = PostfixOpNode::ARGUMENT;
      if (!match(Token::TOK_RPAREN)) {
        fill_expr_list(right);
        consume(Token::TOK_RPAREN);
      }
    } else if (match(Token::TOK_DOT)) {
      ast->v = PostfixOpNode::DOT;
      right = parse_id();
    } else if (match(Token::TOK_TRANSPOSE)) {
      ast->v = PostfixOpNode::TRANSPOSE;
    } else
      break;
    ast->node = Node::POSTFIX_OP;
    ast->children.push_back(std::move(left));
    if (right)
      ast->children.push_back(std::move(right));
    left = std::move(ast);
  }
  return left;
}
inline std::unique_ptr<AST> Parser::parse_primary_expr() {
  auto ast = std::make_unique<AST>();
  if (check(Token::TOK_INT_LIT)) {
    ast->node = Node::INT_LIT;
    ast->v = std::stoll(peek().s);
    advance();
  } else if (check(Token::TOK_FLOAT_LIT)) {
    ast->node = Node::FLOAT_LIT;
    ast->v = std::stod(peek().s);
    advance();
  } else if (match(Token::TOK_TRUE)) {
    ast->node = Node::BOOL;
    ast->v = true;
  } else if (match(Token::TOK_FALSE)) {
    ast->node = Node::BOOL;
    ast->v = false;
  } else if (check(Token::TOK_ID)) {
    ast->node = Node::ID;
    ast->v = peek().s;
    advance();
  } else if (match(Token::TOK_LPAREN)) {
    ast = parse_expr();
    consume(Token::TOK_RPAREN);
  } else if (match(Token::TOK_LBRACKET)) {
    ast->node = Node::TENSOR_LIT;
    if (!match(Token::TOK_RBRACKET)) {
      fill_expr_list(ast);
      consume(Token::TOK_RBRACKET);
    }
  } else {
    error_function();
  }
  return ast;
}
inline void Parser::fill_expr_list(std::unique_ptr<AST> &ast) {
  ast->children.push_back(parse_expr());
  while (match(Token::TOK_COMMA)) {
    ast->children.push_back(parse_expr());
  }
}
inline std::unique_ptr<AST> Parser::parse_const_decl() {
  consume(Token::TOK_CONST);
  auto ast = std::make_unique<AST>();
  ast->node = Node::CONST_DECL;
  ast->children.push_back(parse_id());
  if (match(Token::TOK_COLON)) {
    ast->children.push_back(parse_type());
  }
  consume(Token::TOK_ASSIGN);
  ast->children.push_back(parse_expr());
  consume(Token::TOK_SEMI);
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_function_decl() {
  consume(Token::TOK_FN);
  auto ast = std::make_unique<AST>();
  ast->node = Node::FUNCTION_DECL;
  ast->children.push_back(parse_id());
  consume(Token::TOK_LPAREN);
  if (!match(Token::TOK_RPAREN)) {
    fill_param_list(ast);
    consume(Token::TOK_RPAREN);
  }
  if (match(Token::TOK_ARROW)) {
    ast->children.push_back(parse_type());
  }
  consume(Token::TOK_LBRACE);
  ast->children.push_back(parse_block());
  consume(Token::TOK_RBRACE);
  return ast;
}
inline void Parser::fill_param_list(std::unique_ptr<AST> &ast) {
  ast->children.push_back(parse_param());
  while (match(Token::TOK_COMMA)) {
    ast->children.push_back(parse_param());
  }
}
inline std::unique_ptr<AST> Parser::parse_param() {
  auto ast = std::make_unique<AST>();
  ast->node = Node::PARAM;
  ast->children.push_back(parse_id());
  consume(Token::TOK_COLON);
  ast->children.push_back(parse_type());
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_block() {
  auto ast = std::make_unique<AST>();
  ast->node = Node::BLOCK;
  while (!check(Token::TOK_RBRACE)) {
    if (check(Token::TOK_LET)) {
      ast->children.push_back(parse_var_decl());
    } else if (check(Token::TOK_IF)) {
      ast->children.push_back(parse_if_stmt());
    } else if (check(Token::TOK_WHILE)) {
      ast->children.push_back(parse_while_stmt());
    } else if (check(Token::TOK_FOR)) {
      ast->children.push_back(parse_for_stmt());
    } else if (check(Token::TOK_RETURN)) {
      ast->children.push_back(parse_return_stmt());
    } else if (check(Token::TOK_PRINT)) {
      ast->children.push_back(parse_print_stmt());
    } else {
      ast->children.push_back(parse_expr_stmt());
    }
  }
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_var_decl() {
  consume(Token::TOK_LET);
  auto ast = std::make_unique<AST>();
  ast->node = Node::VAR_DECL;
  ast->children.push_back(parse_id());
  if (match(Token::TOK_COLON)) {
    ast->children.push_back(parse_type());
    if (match(Token::TOK_ASSIGN)) {
      ast->children.push_back(parse_expr());
    }
  } else {
    consume(Token::TOK_ASSIGN);
    ast->children.push_back(parse_expr());
  }
  consume(Token::TOK_SEMI);
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_if_stmt() {
  consume(Token::TOK_IF);
  auto ast = std::make_unique<AST>();
  ast->node = Node::IF_STMT;
  ast->children.push_back(parse_expr());
  consume(Token::TOK_LBRACE);
  ast->children.push_back(parse_block());
  consume(Token::TOK_RBRACE);
  if (match(Token::TOK_ELSE)) {
    consume(Token::TOK_LBRACE);
    ast->children.push_back(parse_block());
    consume(Token::TOK_RBRACE);
  }
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_while_stmt() {
  consume(Token::TOK_WHILE);
  auto ast = std::make_unique<AST>();
  ast->node = Node::WHILE_STMT;
  ast->children.push_back(parse_expr());
  consume(Token::TOK_LBRACE);
  ast->children.push_back(parse_block());
  consume(Token::TOK_RBRACE);
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_for_stmt() {
  consume(Token::TOK_FOR);
  auto ast = std::make_unique<AST>();
  ast->node = Node::FOR_STMT;
  ast->children.push_back(parse_id());
  consume(Token::TOK_IN);
  ast->children.push_back(parse_expr());
  consume(Token::TOK_LBRACE);
  ast->children.push_back(parse_block());
  consume(Token::TOK_RBRACE);
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_return_stmt() {
  consume(Token::TOK_RETURN);
  auto ast = std::make_unique<AST>();
  ast->node = Node::RETURN_STMT;
  if (!match(Token::TOK_SEMI)) {
    ast->children.push_back(parse_expr());
    consume(Token::TOK_SEMI);
  }
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_print_stmt() {
  auto ast = std::make_unique<AST>();
  ast->node = Node::PRINT_STMT;
  if (match(Token::TOK_PRINT)) {
    ast->v = PrintNode::PRINT;
  } else {
    consume(Token::TOK_PRINTLN);
    ast->v = PrintNode::PRINTLN;
  }
  ast->children.push_back(parse_expr());
  consume(Token::TOK_SEMI);
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_expr_stmt() {
  auto ast = std::make_unique<AST>();
  ast->node = Node::EXPR_STMT;
  ast->children.push_back(parse_expr());
  consume(Token::TOK_SEMI);
  return ast;
}
inline std::unique_ptr<AST> Parser::parse_extern_fn_decl() {
  auto ast = std::make_unique<AST>();
  ast->node = Node::EXTERN_DECL;
  consume(Token::TOK_EXTERN);
  consume(Token::TOK_FN);
  ast->children.push_back(parse_id());
  consume(Token::TOK_LPAREN);
  if (!match(Token::TOK_RPAREN)) {
    fill_param_list(ast);
    consume(Token::TOK_RPAREN);
  }
  if (match(Token::TOK_ARROW)) {
    ast->children.push_back(parse_type());
  }
  consume(Token::TOK_SEMI);
  return ast;
}
