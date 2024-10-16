#include "lexer.h"
#include "../errors/errors.h"
#include <cctype>

const std::unordered_set<std::string> Lexer::keywords = {
    "dataset", "fn", "for", "in", "return", "if", "else", "while",
    "Int", "Float", "Void","Vector", "Matrix","to","Dataset"
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
            if (std::isdigit(c)) {
                Token numberToken = scanNumber();
                if (std::isalpha(peek()) || peek() == '_') {
                    // If the next character is a letter or underscore, it's an invalid identifier
                    std::string invalidIdentifier = numberToken.value;
                    while (std::isalnum(peek()) || peek() == '_') {
                        invalidIdentifier += advance();
                    }
                    LexerError::invalidIdentifier("input", line, column - invalidIdentifier.length(), invalidIdentifier);
                    return createToken(TokenType::UNKNOWN, invalidIdentifier);
                }
                return numberToken;
            }
            LexerError::unexpectedCharacter("input", line, column - 1, c);
            return createToken(TokenType::UNKNOWN, std::string(1, c));


    }
}

Token Lexer::scanIdentifierOrKeyword() {
    std::string value;
    value += input[position - 1];  

    while (std::isalnum(peek()) || peek() == '_') {
        value += advance();
    }

    if (keywords.find(value) != keywords.end()) {
        return createToken(TokenType::KEYWORD, value);
    }

    if (!std::isalpha(value[0]) && value[0] != '_') {
        LexerError::invalidIdentifier("input", line, column - value.length(), value);
        return createToken(TokenType::UNKNOWN, value);
    }

    return createToken(TokenType::IDENTIFIER, value);
}

Token Lexer::scanNumber() {
    std::string value;
    value += input[position - 1];
    bool hasDecimalPoint = false;

    while (std::isdigit(peek()) || peek() == '.') {
        if (peek() == '.') {
            if (hasDecimalPoint) {
                value += advance();
                LexerError::invalidNumber("input", line, column - value.length(), value);
                return createToken(TokenType::UNKNOWN, value);
            }
            hasDecimalPoint = true;
        }
        value += advance();
    }

    // Check if the next character is a letter or underscore
    if (std::isalpha(peek()) || peek() == '_') {
        return createToken(TokenType::UNKNOWN, value);
    }

    return createToken(TokenType::LITERAL, value);
}


Token Lexer::scanString() {
    std::string value;
    int startLine = line;
    int startColumn = column - 1;

    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        value += advance();
    }

    if (isAtEnd()) {
        LexerError::unterminatedString("input", startLine, startColumn);
        return createToken(TokenType::UNKNOWN, value);
    }

    advance();  // Consume the closing "
    return createToken(TokenType::LITERAL, value);
}


void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ': case '\r': case '\t':
                advance();
                break;
            case '\n':
                line++;
                column = 1;
                advance();
                break;
            default:
                return;
        }
    }
}

void Lexer::skipComment() {
    while (peek() != '\n' && !isAtEnd()) advance();
}

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::LITERAL: return "LITERAL";
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::DELIMITER: return "DELIMITER";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "UNKNOWN";
    }
}