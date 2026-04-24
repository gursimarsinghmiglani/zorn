#pragma once
#include "token.hpp"
#include <string>
struct Lexeme {
  int start = -1;
  int length = 0;
  int line_number = -1;
  std::string s = "";
  Token tok = Token::TOK_ERROR;
  Lexeme() = default;
  Lexeme(int start, int length, int line_number, std::string s, Token tok)
      : start(start), length(length), line_number(line_number), s(s), tok(tok) {
  }
};
