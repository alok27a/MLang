#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <regex>

// Token structure
struct Token
{
    std::string type;
    std::string value;
    int line;
    int column;

    Token(const std::string &type, const std::string &value, int line, int column)
        : type(type), value(value), line(line), column(column) {}
};

// Function to read tokens from the lexer file
std::vector<Token> readTokensFromFile(const std::string &fileName)
{
    std::vector<Token> tokens;
    std::ifstream file(fileName);

    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return tokens;
    }

    std::string line;
    std::regex tokenPattern(R"(<(\w+),\s*\"(.*?)\">\s*\[Line:\s*(\d+),\s*Column:\s*(-?\d+)\])");

    while (std::getline(file, line))
    {
        std::smatch match;
        if (std::regex_search(line, match, tokenPattern) && match.size() == 5)
        {
            std::string type = match[1].str();
            std::string value = match[2].str();
            int lineNum = std::stoi(match[3].str());
            int columnNum = std::stoi(match[4].str());

            tokens.emplace_back(type, value, lineNum, columnNum);
        }
        else
        {
            std::cerr << "Error parsing token from line: " << line << std::endl;
        }
    }

    if (tokens.empty())
    {
        std::cerr << "No tokens read from file: " << fileName << ". Please check the file format." << std::endl;
    }
    else
    {
        std::cout << "Successfully read " << tokens.size() << " tokens." << std::endl;
    }

    return tokens;
}

// Base class for all AST nodes
class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void print(int indentLevel = 0) const = 0; // For debugging with indentation
};

// Represents the overall program with a list of functions
class ProgramNode : public ASTNode
{
public:
    std::vector<std::unique_ptr<ASTNode>> functions;

    void addFunction(std::unique_ptr<ASTNode> func)
    {
        functions.push_back(std::move(func));
    }

    void print(int indentLevel = 0) const override
    {
        for (const auto &func : functions)
        {
            func->print(indentLevel);
        }
    }
};

// Represents a function definition
class FunctionNode : public ASTNode
{
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters;
    std::string returnType;
    std::unique_ptr<ASTNode> body;

    FunctionNode(const std::string &name, const std::vector<std::pair<std::string, std::string>> &params, const std::string &returnType, std::unique_ptr<ASTNode> body)
        : name(name), parameters(params), returnType(returnType), body(std::move(body)) {}

    void print(int indentLevel = 0) const override
    {
        std::cout << std::string(indentLevel, ' ') << "FUNCTION_DEFINITION\n";
        std::cout << std::string(indentLevel + 2, ' ') << "FUNCTION_NAME: " << name << "\n";
        std::cout << std::string(indentLevel + 2, ' ') << "RETURN_TYPE: " << returnType << "\n";
        std::cout << std::string(indentLevel + 2, ' ') << "PARAMETERS\n";
        for (const auto &param : parameters)
        {
            std::cout << std::string(indentLevel + 4, ' ') << "PARAMETER: " << param.first << " (TYPE: " << param.second << ")\n";
        }
        if (body)
        {
            body->print(indentLevel + 2);
        }
    }
};

// Represents a block of statements (function body, etc.)
class BlockNode : public ASTNode
{
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void addStatement(std::unique_ptr<ASTNode> stmt)
    {
        statements.push_back(std::move(stmt));
    }

    void print(int indentLevel = 0) const override
    {
        std::cout << std::string(indentLevel, ' ') << "FUNCTION_BODY\n";
        for (const auto &stmt : statements)
        {
            stmt->print(indentLevel + 2);
        }
    }
};

// Represents a variable declaration
class VariableDeclarationNode : public ASTNode
{
public:
    std::string type;
    std::string name;
    std::unique_ptr<ASTNode> initializer;

    VariableDeclarationNode(const std::string &type, const std::string &name, std::unique_ptr<ASTNode> initializer = nullptr)
        : type(type), name(name), initializer(std::move(initializer)) {}

    void print(int indentLevel = 0) const override
    {
        std::cout << std::string(indentLevel, ' ') << "VARIABLE_DECLARATION\n";
        std::cout << std::string(indentLevel + 2, ' ') << "IDENTIFIER: " << name << " (TYPE: " << type << ")\n";
        if (initializer)
        {
            initializer->print(indentLevel + 2);
        }
    }
};

// Represents a function call
class FunctionCallNode : public ASTNode
{
public:
    std::string functionName;
    std::vector<std::unique_ptr<ASTNode>> arguments;

    FunctionCallNode(const std::string &functionName) : functionName(functionName) {}

    void addArgument(std::unique_ptr<ASTNode> arg)
    {
        arguments.push_back(std::move(arg));
    }

    void print(int indentLevel = 0) const override
    {
        std::cout << std::string(indentLevel, ' ') << "FUNCTION_CALL: " << functionName << "\n";
        if (!arguments.empty())
        {
            std::cout << std::string(indentLevel + 2, ' ') << "ARGUMENTS\n";
            for (const auto &arg : arguments)
            {
                arg->print(indentLevel + 4);
            }
        }
    }
};

// Represents a literal value (like numbers or strings)
class LiteralNode : public ASTNode
{
public:
    std::string value;

    LiteralNode(const std::string &value) : value(value) {}

    void print(int indentLevel = 0) const override
    {
        std::cout << std::string(indentLevel, ' ') << "LITERAL_VALUE: " << value << "\n";
    }
};

// Represents an assignment statement
class AssignmentNode : public ASTNode
{
public:
    std::string variableName;
    std::unique_ptr<ASTNode> expression;

    AssignmentNode(const std::string &variableName, std::unique_ptr<ASTNode> expression)
        : variableName(variableName), expression(std::move(expression)) {}

    void print(int indentLevel = 0) const override
    {
        std::cout << std::string(indentLevel, ' ') << "ASSIGNMENT_EXPRESSION\n";
        std::cout << std::string(indentLevel + 2, ' ') << "IDENTIFIER: " << variableName << "\n";
        std::cout << std::string(indentLevel + 2, ' ') << "EXPRESSION\n";
        if (expression)
        {
            expression->print(indentLevel + 4);
        }
    }
};

class BinaryOperatorNode : public ASTNode
{
public:
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOperatorNode(const std::string &op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    void print(int indentLevel = 0) const override
    {
        std::cout << std::string(indentLevel, ' ') << "OPERATOR: " << op << "\n";
        left->print(indentLevel + 2);
        right->print(indentLevel + 2);
    }
};

// Represents a range in a for loop
class ForLoopNode : public ASTNode
{
public:
    std::string loopVar;
    std::string loopVarType;
    std::unique_ptr<ASTNode> rangeStart;
    std::unique_ptr<ASTNode> rangeEnd;
    std::unique_ptr<ASTNode> body;

    ForLoopNode(const std::string &loopVar, const std::string &loopVarType,
                std::unique_ptr<ASTNode> rangeStart, std::unique_ptr<ASTNode> rangeEnd,
                std::unique_ptr<ASTNode> body)
        : loopVar(loopVar), loopVarType(loopVarType), rangeStart(std::move(rangeStart)),
          rangeEnd(std::move(rangeEnd)), body(std::move(body)) {}

    void print(int indentLevel = 0) const override
    {
        std::cout << std::string(indentLevel, ' ') << "FOR_LOOP\n";
        std::cout << std::string(indentLevel + 2, ' ') << "LOOP_VARIABLE: " << loopVar << " (TYPE: " << loopVarType << ")\n";
        std::cout << std::string(indentLevel + 2, ' ') << "RANGE_START: ";
        rangeStart->print(0);
        std::cout << std::string(indentLevel + 2, ' ') << "RANGE_END: ";
        rangeEnd->print(0);
        std::cout << std::string(indentLevel + 2, ' ') << "LOOP_BODY\n";
        body->print(indentLevel + 4);
    }
};

std::unique_ptr<ASTNode> parseExpression(const std::vector<Token> &tokens, size_t &index)
{
    // Ensure there are enough tokens to form an expression
    if (index >= tokens.size())
    {
        std::cerr << "Error: Unexpected end of tokens while parsing expression." << std::endl;
        return nullptr;
    }

    // Start parsing with the left operand (should be a literal for now)
    std::unique_ptr<ASTNode> left = std::make_unique<LiteralNode>(tokens[index].value);
    ++index;

    while (index < tokens.size() && tokens[index].type == "OPERATOR")
    {
        std::string op = tokens[index].value;
        ++index;

        // Ensure the right operand exists
        if (index >= tokens.size())
        {
            std::cerr << "Error: Operator found but no right operand." << std::endl;
            return nullptr;
        }

        auto right = std::make_unique<LiteralNode>(tokens[index].value);
        ++index;

        // Create a binary operator node and update the left node
        auto binaryOp = std::make_unique<BinaryOperatorNode>(op, std::move(left), std::move(right));
        left = std::move(binaryOp);
    }

    return left;
}

class ReturnNode : public ASTNode
{
public:
    std::unique_ptr<ASTNode> expression;

    ReturnNode(std::unique_ptr<ASTNode> expr) : expression(std::move(expr)) {}

    void print(int indentLevel = 0) const override
    {
        std::cout << std::string(indentLevel, ' ') << "RETURN_STATEMENT\n";
        if (expression)
        {
            expression->print(indentLevel + 2);
        }
    }
};

class ParseException : public std::runtime_error
{
public:
    ParseException(const std::string &message) : std::runtime_error(message) {}
};

std::unique_ptr<ProgramNode> parseTokens(const std::vector<Token> &tokens)
{
    auto program = std::make_unique<ProgramNode>();
    size_t index = 0;

    while (index < tokens.size())
    {
        const Token &token = tokens[index];
        if (token.type == "KEYWORD" && token.value == "fn")
        {
            std::string functionName = tokens[++index].value;
            ++index; // skip '('
            std::vector<std::pair<std::string, std::string>> parameters;
            ++index; // skip '('

            while (index < tokens.size() && tokens[index].value != ")")
            {
                std::string paramName = tokens[index++].value;
                std::string paramType;
                if (index < tokens.size() && tokens[index].value == ":")
                {
                    ++index; // skip ':'
                    int nestedAngleBrackets = 0;
                    while (index < tokens.size())
                    {
                        if (tokens[index].value == "<")
                        {
                            nestedAngleBrackets++;
                        }
                        else if (tokens[index].value == ">")
                        {
                            nestedAngleBrackets--;
                        }
                        else if (tokens[index].value == "," && nestedAngleBrackets == 0)
                        {
                            break;
                        }
                        else if (tokens[index].value == ")" && nestedAngleBrackets == 0)
                        {
                            break;
                        }
                        paramType += tokens[index].value;
                        ++index;
                    }
                }
                parameters.emplace_back(paramName, paramType);
                if (tokens[index].value == ",")
                    ++index;
            }
            ++index; // skip ')'

            // Parse return type, only if the next token is '->'
            std::string returnType;
            if (tokens[index].value == "->")
            {
                ++index; // skip '->'
                int nestedAngleBrackets = 0;
                while (index < tokens.size())
                {
                    if (tokens[index].value == "<")
                    {
                        nestedAngleBrackets++;
                    }
                    else if (tokens[index].value == ">")
                    {
                        nestedAngleBrackets--;
                        if (nestedAngleBrackets == 0)
                        {
                            returnType += tokens[index].value;
                            ++index;
                            break;
                        }
                    }
                    else if (tokens[index].value == "{" && nestedAngleBrackets == 0)
                    {
                        break;
                    }
                    returnType += tokens[index].value;
                    ++index;
                }
            }

            // Parse function body, starting with '{'
            if (tokens[index].value == "{")
                ++index; // skip '{'
            auto body = std::make_unique<BlockNode>();
            int braceCount = 1;

            while (index < tokens.size())
            {
                const Token &currentToken = tokens[index];

                if (currentToken.value == "{")
                {
                    ++braceCount;
                }
                else if (currentToken.value == "}")
                {
                    --braceCount;
                    if (braceCount == 0)
                    {
                        ++index;
                        break;
                    }
                }
                else if (currentToken.type == "KEYWORD" && currentToken.value == "for")
                {
                    ++index; // Skip 'for'
                    std::string loopVar = tokens[index].value;
                    std::string loopVarType = "Int"; // Assume Int for simplicity
                    ++index;                         // Move past the variable
                    ++index;                         // Skip 'in'

                    auto rangeStart = parseExpression(tokens, index);

                    // Check for the "to" keyword
                    if (index >= tokens.size() || tokens[index].value != "to")
                    {
                        throw ParseException("Expected 'to' after range start expression in 'for' loop at line " +
                                             std::to_string(tokens[index].line) + ", column " +
                                             std::to_string(tokens[index].column) + ".");
                    }
                    ++index; // Skip 'to'

                    // Check if rangeEnd is present after 'to'
                    if (index >= tokens.size())
                    {
                        throw ParseException("Expected range end expression in 'for' loop but found end of input at line " +
                                             std::to_string(tokens[index - 1].line) + ", column " +
                                             std::to_string(tokens[index - 1].column) + ".");
                    }
                    auto rangeEnd = parseExpression(tokens, index);
                    if (!rangeEnd)
                    {
                        throw ParseException("Expected range end expression in 'for' loop but found '" + tokens[index].value +
                                             "' at line " + std::to_string(tokens[index].line) + ", column " +
                                             std::to_string(tokens[index].column) + ".");
                    }

                    auto loopBody = std::make_unique<BlockNode>();
                    if (index < tokens.size() && tokens[index].value == "{")
                    {
                        ++index; // Skip '{'
                        int bodyBraceCount = 1;
                        while (index < tokens.size() && bodyBraceCount > 0)
                        {
                            if (tokens[index].value == "{")
                            {
                                ++bodyBraceCount;
                            }
                            else if (tokens[index].value == "}")
                            {
                                --bodyBraceCount;
                                if (bodyBraceCount == 0)
                                {
                                    break;
                                }
                            }
                            else if (tokens[index].type == "IDENTIFIER")
                            {
                                std::string varName = tokens[index].value;
                                if (index + 1 < tokens.size() && tokens[index + 1].value == "=")
                                {
                                    index += 2; // Skip '='
                                    auto expr = parseExpression(tokens, index);
                                    loopBody->addStatement(std::make_unique<AssignmentNode>(varName, std::move(expr)));
                                }
                            }
                            if (bodyBraceCount > 0)
                                ++index;
                        }
                    }
                    body->addStatement(std::make_unique<ForLoopNode>(loopVar, loopVarType, std::move(rangeStart), std::move(rangeEnd), std::move(loopBody)));
                }
                else if (currentToken.type == "KEYWORD" && currentToken.value == "return")
                {
                    ++index; // Skip 'return'

                    // Attempt to parse an expression following the 'return' keyword
                    auto expr = parseExpression(tokens, index);
                    body->addStatement(std::make_unique<ReturnNode>(std::move(expr)));

                    // Check for semicolon after the return statement
                    if (index < tokens.size() && tokens[index].value == ";")
                    {
                        ++index; // Skip the semicolon
                    }
                    else
                    {
                        throw ParseException("Expected semicolon after return statement at line " +
                                             std::to_string(tokens[index - 1].line) + ", column " +
                                             std::to_string(tokens[index - 1].column) + ".");
                    }
                }

                else
                {
                    ++index; // Move to next token
                }
            }

            // Check for imbalance in braces
            if (braceCount != 0)
            {
                throw ParseException("Mismatched braces detected in function body.");
            }

            program->addFunction(std::make_unique<FunctionNode>(functionName, parameters, returnType, std::move(body)));
        }

        else
        {
            ++index; // Move to next token if it's not a function
        }
    }

    return program;
}

int main(int argc, char *argv[])
{
    // Check if both input and output filenames are provided as arguments
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    // Get the input and output filenames from command-line arguments
    std::string inputFileName = argv[1];
    std::string outputFileName = argv[2];

    // Read tokens from the input file
    std::vector<Token> tokens = readTokensFromFile(inputFileName);
    if (tokens.empty())
    {
        std::cerr << "No tokens found in the input file." << std::endl;
        return 1;
    }

    // Parse the tokens to create an AST
    std::unique_ptr<ProgramNode> ast = parseTokens(tokens);

    // Open the output file
    std::ofstream outputFile(outputFileName);
    if (!outputFile.is_open())
    {
        std::cerr << "Failed to open output file: " << outputFileName << std::endl;
        return 1;
    }

    // Redirect cout to the file
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(outputFile.rdbuf());

    // Print the AST to the file
    ast->print();

    // Restore cout to its original buffer
    std::cout.rdbuf(coutbuf);

    // Close the output file
    outputFile.close();

    std::cout << "AST output has been saved to " << outputFileName << std::endl;

    return 0;
}