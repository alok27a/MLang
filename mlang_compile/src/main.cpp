#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"

int main() {
    std::string filename = "input.txt";
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string input = buffer.str();

    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();

    for (const auto& token : tokens) {
        std::cout << "<"<<tokenTypeToString(token.type) << ", \""<< token.value << "\"> [Line: " << token.line << ", Column: " << token.column << "]" << std::endl;
    }

    return 0;
}