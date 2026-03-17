#include "lexer.hpp"
int main() {
    std::string source = "while (true) a= .5; \nbrea$k; continue; A*B;";
    std::vector<Lexeme> lexemes = maximal_munch(source);
    for (Lexeme lex: lexemes) {
        std::cout << source.substr(lex.start, lex.length) <<  " "  << token_names[static_cast<int>(lex.tok)] << "\n";
    }
} 
