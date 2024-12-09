#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_filename>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string input = buffer.str();

    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    for (const auto& token : tokens) {
        cout << "<" << tokenTypeToString(token.type) << ", \"" << token.value << "\"> [Line: " << token.line << ", Column: " << token.column << "]" << endl;
    }

    return 0;
}
