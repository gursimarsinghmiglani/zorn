#pragma once
#include "token.hpp"
struct Lexeme {
    int start;
    int length;
    int line_number;
    Token tok;
    Lexeme(int start, int length, int line_number, Token tok): start(start), length(length), line_number(line_number), tok(tok) {}
};
