#include "lexer.h"
#include "errors.h"
#include <cctype>

const std::unordered_set<std::string> Lexer::keywords = {
    "dataset", "fn", "for", "in", "return", "if", "else", "while",
    "Int", "Float", "Vector", "Matrix","to","Dataset"
};

Lexer::Lexer(const std::string& input) : input(input), position(0), line(1), column(1) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        tokens.push_back(scanToken());
    }
    tokens.push_back(createToken(TokenType::END_OF_FILE, ""));
    return tokens;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return input[position];
}

char Lexer::advance() {
    position++;
    column++;
    return input[position - 1];
}

bool Lexer::isAtEnd() { 
    return position >= input.length();
}

Token Lexer::createToken(TokenType type, const std::string& value) {
    return {type, value, line, column - static_cast<int>(value.length())};
}

Token Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': case ')': case '{': case '}': case '[': case ']':
        case ',': case ':': case ';': case '<': case '>':
            return createToken(TokenType::DELIMITER, std::string(1, c));
        case '.': 
            if (peek() == '.') {
                advance();
                return createToken(TokenType::OPERATOR, "..");
            }
            return createToken(TokenType::DELIMITER, ".");
        case '+': case '*': case '=':
            return createToken(TokenType::OPERATOR, std::string(1, c));
        case '-': 
            if (peek() == '>') {
                advance();
                return createToken(TokenType::OPERATOR, "->");
            }
            return createToken(TokenType::OPERATOR, "-");
        case '/': 
            if (peek() == '/') {
                skipComment();
                return createToken(TokenType::COMMENT, "");
            }
            return createToken(TokenType::OPERATOR, "/");
        case '"': return scanString();
        case '@':
            LexerError::unexpectedCharacter("input", line, column - 1, c);
            return createToken(TokenType::UNKNOWN, std::string(1, c));
        default:
            if (std::isalpha(c) || c == '_') return scanIdentifierOrKeyword();
            if (std::isdigit(c)) return scanNumber();
            LexerError::unexpectedCharacter("input", line, column - 1, c);
            return createToken(TokenType::UNKNOWN, std::string(1, c));

    }
}


