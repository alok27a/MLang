#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_set>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    LITERAL,
    OPERATOR,
    DELIMITER,
    COMMENT,
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
public:
    Lexer(const std::string& input);
    std::vector<Token> tokenize();

private:
    std::string input;
    int position;
    int line;
    int column;

    char peek();
    char advance();
    bool isAtEnd();
    Token createToken(TokenType type, const std::string& value);
    Token scanToken();
    Token scanIdentifierOrKeyword();
    Token scanNumber();
    Token scanString();
    Token scanPossibleGenericType();
    void skipWhitespace();
    void skipComment();

    static const std::unordered_set<std::string> keywords;
};

std::string tokenTypeToString(TokenType type);

#endif 