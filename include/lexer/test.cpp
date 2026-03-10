#include "lexer.hpp"

int main() {
    std::string source = "while true: 43534; \n838383u\nfor i in range(10): break; end; 55; print(00) [] {}";
    std::vector<Lexeme> lexemes = maximal_munch(source);
    for (Lexeme lexeme: lexemes) {
        std::cout << source.substr(lexeme.start, lexeme.length) << " " << token_names[static_cast<int>(lexeme.tok)] << "\n";
    }
}
