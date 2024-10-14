#ifndef ERRORS_H
#define ERRORS_H

#include <string>

class LexerError {
public:
    static void report(const std::string& filename, int line, int column, const std::string& message);
    static void unexpectedCharacter(const std::string& filename, int line, int column, char c);
    static void unterminatedString(const std::string& filename, int line, int column);
    static void invalidNumber(const std::string& filename, int line, int column, const std::string& number);
    static void invalidIdentifier(const std::string& filename, int line, int column, const std::string& identifier);

private:
    static std::string formatMessage(const std::string& message, const std::string& token);
};

#endif // ERRORS_H