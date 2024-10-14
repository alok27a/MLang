#include "errors.h"
#include <iostream>
#include <sstream>

void LexerError::report(const std::string& filename, int line, int column, const std::string& message) {
    std::cerr << filename << ":" << line << ":" << column << ": error: " << message << std::endl;
}

std::string LexerError::formatMessage(const std::string& message, const std::string& token) {
    std::ostringstream oss;
    oss << message;
    if (token.empty()) {
        oss << "'" << token << "'";
    }
    return oss.str();
}

void LexerError::unexpectedCharacter(const std::string& filename, int line, int column, char c) {
    report(filename, line, column, formatMessage("Unexpected character", std::string(1, c)));
}

void LexerError::unterminatedString(const std::string& filename, int line, int column) {
    report(filename, line, column, "Unterminated string literal");
}

void LexerError::invalidNumber(const std::string& filename, int line, int column, const std::string& number) {
    report(filename, line, column, formatMessage("Invalid number", number));
}

void LexerError::invalidIdentifier(const std::string& filename, int line, int column, const std::string& identifier) {
    report(filename, line, column, formatMessage("Invalid identifier", identifier));
}