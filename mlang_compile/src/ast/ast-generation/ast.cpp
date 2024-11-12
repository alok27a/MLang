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


int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
