#pragma once
#include <string>
#include "token.hpp"
struct Lexeme {
    int start;
    int length;
    int line_number;
    std::string s;
    Token tok;
    Lexeme(int start, int length, int line_number, std::string s, Token tok): start(start), length(length), line_number(line_number), s(s), tok(tok) {}
};
