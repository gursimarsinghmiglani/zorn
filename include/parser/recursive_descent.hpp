#pragma once
#include "ast.hpp"
#include "../lexer/lexeme.hpp"
int input_ptr = 0;
inline AST *program(std::vector<Lexeme> lexemes) {
    AST ast;
    ast.node = Node::PROGRAM;
    while (input_ptr < lexemes.size()) {
        if (lexemes[input_ptr].tok == Token::TOK_GLOBAL) {
            AST *global_decl_ptr = global_decl(lexemes);
            if (global_decl_ptr) {
                ast.children.push_back(std::unique_ptr<AST>(global_decl_ptr));
            }
        }
        AST *const_decl_ptr = const_decl();
        if (const_decl_ptr) {
            ast.children.push_back(std::unique_ptr<AST>(const_decl_ptr));
        }
        AST *function_decl_ptr = function_decl(lexemes);
        if (function_decl_ptr) {
            ast.children.push_back(std::unique_ptr<AST>(function_decl_ptr));
        }
    }
    return input_ptr == lexemes.size() ? 
}
