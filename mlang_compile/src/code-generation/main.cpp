#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <stack>
#include <algorithm>

class ASTNode
{
public:
    std::string type;
    std::string value;
    std::vector<ASTNode *> children;

    ASTNode(const std::string &t, const std::string &v = "") : type(t), value(v) {}
    ~ASTNode()
    {
        for (auto child : children)
        {
            delete child;
        }
    }
};

class ASTPythonGenerator
{
private:
    int indentLevel = 0;
    std::string getIndent()
    {
        return std::string(indentLevel * 4, ' ');
    }

    std::string simplifyExpression(const std::string &expr)
    {
        // Simple optimizations
        if (expr == "0 + 0")
            return "0";
        if (expr == "a + 0" || expr == "0 + a")
            return "a";
        if (expr == "a * 1" || expr == "1 * a")
            return "a";
        if (expr == "a * 0" || expr == "0 * a")
            return "0";

        // Constant folding
        std::istringstream iss(expr);
        std::string token;
        std::vector<std::string> tokens;

        while (iss >> token)
        {
            tokens.push_back(token);
        }

        if (tokens.size() == 3 && (tokens[1] == "+" || tokens[1] == "-" || tokens[1] == "*" || tokens[1] == "/"))
        {
            try
            {
                int lhs = std::stoi(tokens[0]);
                int rhs = std::stoi(tokens[2]);
                if (tokens[1] == "+")
                    return std::to_string(lhs + rhs);
                if (tokens[1] == "-")
                    return std::to_string(lhs - rhs);
                if (tokens[1] == "*")
                    return std::to_string(lhs * rhs);
                if (tokens[1] == "/" && rhs != 0)
                    return std::to_string(lhs / rhs);
            }
            catch (const std::exception &)
            {
                // Non-integer expressions are skipped
            }
        }

        return expr; // Return the original if no optimization is applied
    }

public:
    std::string generatePython(ASTNode *node)
    {
        if (!node)
            return "";
        std::ostringstream python;

        if (node->type == "FUNCTION_DEFINITION")
        {
            python << generateFunctionDefinition(node);
            indentLevel = 0;
        }
        else if (node->type == "FOR_LOOP")
        {
            python << generateForLoop(node);
        }
        else if (node->type == "ASSIGNMENT_EXPRESSION")
        {
            python << generateAssignment(node);
        }
        else if (node->type == "RETURN_STATEMENT")
        {
            python << generateReturnStatement(node);
        }
        else
        {
            for (auto child : node->children)
            {
                python << generatePython(child);
            }
        }

        return python.str();
    }

    std::string generateFunctionDefinition(ASTNode *node)
    {
        std::ostringstream python;
        std::string funcName, returnType;
        std::vector<std::string> params;

        for (auto child : node->children)
        {
            if (child->type == "FUNCTION_NAME")
            {
                funcName = child->value;
            }
            else if (child->type == "RETURN_TYPE")
            {
                returnType = child->value;
                std::transform(returnType.begin(), returnType.end(), returnType.begin(),
                               [](unsigned char c)
                               { return std::tolower(c); });
            }
            else if (child->type == "PARAMETERS")
            {
                for (auto param : child->children)
                {
                    std::string paramName = param->value;
                    std::string paramType = "";
                    size_t typePos = paramName.find(" (TYPE:");
                    if (typePos != std::string::npos)
                    {
                        paramType = paramName.substr(typePos + 7);
                        paramName = paramName.substr(0, typePos);
                        if (!paramType.empty() && paramType.back() == ')')
                        {
                            paramType.pop_back();
                        }
                        std::transform(paramType.begin(), paramType.end(), paramType.begin(),
                                       [](unsigned char c)
                                       { return std::tolower(c); });
                        params.push_back(paramName + ": " + paramType);
                    }
                    else
                    {
                        params.push_back(paramName);
                    }
                }
            }
        }

        python << getIndent() << "def " << funcName << "(" << join(params, ", ") << ") -> " << returnType << ":\n";
        indentLevel++;
        for (auto child : node->children)
        {
            if (child->type == "FUNCTION_BODY")
            {
                python << generatePython(child);
            }
        }
        indentLevel--;
        return python.str();
    }

    std::string generateForLoop(ASTNode *node)
    {
        std::ostringstream python;
        std::string loopVar, rangeStart, rangeEnd;

        for (auto child : node->children)
        {
            if (child->type == "LOOP_VARIABLE")
            {
                std::string fullVar = child->value;
                size_t pos = fullVar.find(" (TYPE:");
                loopVar = (pos != std::string::npos) ? fullVar.substr(0, pos) : fullVar;
            }
            else if (child->type == "RANGE_START")
            {
                rangeStart = child->value.find("LITERAL_VALUE") != std::string::npos ? child->value.substr(child->value.find(":") + 1) : (!child->children.empty() ? child->children[0]->value : "");
            }
            else if (child->type == "RANGE_END")
            {
                rangeEnd = child->value.find("LITERAL_VALUE") != std::string::npos ? child->value.substr(child->value.find(":") + 1) : (!child->children.empty() ? child->children[0]->value : "");
            }
        }

        if (loopVar.empty() || rangeStart.empty() || rangeEnd.empty())
        {
            return "";
        }

        python << getIndent() << "for " << loopVar << " in range(" << rangeStart << ", " << rangeEnd << "):\n";
        indentLevel++;

        for (auto child : node->children)
        {
            if (child->type == "LOOP_BODY")
            {
                for (auto loopBodyChild : child->children)
                {
                    python << generatePython(loopBodyChild);
                }
            }
        }

        indentLevel--;
        return python.str();
    }

    std::string generateExpression(ASTNode *node)
    {
        std::ostringstream expression;
        if (node->type == "EXPRESSION")
        {
            for (auto child : node->children)
            {
                expression << generateExpression(child);
            }
        }
        else if (node->type == "OPERATOR")
        {
            if (node->children.size() == 2)
            {
                std::string left = generateExpression(node->children[0]);
                std::string right = generateExpression(node->children[1]);
                expression << simplifyExpression(left + " " + node->value + " " + right);
            }
        }
        else if (node->type == "LITERAL_VALUE" || node->type == "IDENTIFIER")
        {
            expression << node->value;
        }
        return expression.str();
    }

    std::string generateAssignment(ASTNode *node)
    {
        std::ostringstream python;
        std::string variable;
        std::string value;

        for (auto child : node->children)
        {
            if (child->type == "IDENTIFIER")
            {
                variable = child->value;
            }
            if (child->type == "EXPRESSION")
            {
                value = generateExpression(child);
            }
        }

        if (!variable.empty() && !value.empty())
        {
            python << getIndent() << variable << " = " << simplifyExpression(value) << "\n";
        }

        return python.str();
    }

    std::string generateReturnStatement(ASTNode *node)
    {
        std::ostringstream python;
        std::string expr = generateExpression(node->children[0]);
        python << getIndent() << "return " << simplifyExpression(expr) << "\n";
        return python.str();
    }

    std::string join(const std::vector<std::string> &vec, const std::string &delim)
    {
        std::ostringstream result;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (i > 0)
                result << delim;
            result << vec[i];
        }
        return result.str();
    }
};

ASTNode *parseASTFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return nullptr;
    }

    std::stack<ASTNode *> nodeStack;
    ASTNode *root = new ASTNode("ROOT");
    nodeStack.push(root);

    std::string line;
    int prevIndent = -1;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        int indent = 0;
        while (indent < line.length() && line[indent] == ' ')
        {
            indent++;
        }
        indent /= 2;

        line = line.substr(line.find_first_not_of(" \t"));
        if (line.empty())
            continue;

        size_t colonPos = line.find(':');
        ASTNode *newNode;
        if (colonPos != std::string::npos)
        {
            newNode = new ASTNode(line.substr(0, colonPos), line.substr(colonPos + 2));
        }
        else
        {
            newNode = new ASTNode(line);
        }

        while (indent <= prevIndent && !nodeStack.empty())
        {
            nodeStack.pop();
            prevIndent--;
        }

        if (!nodeStack.empty())
        {
            nodeStack.top()->children.push_back(newNode);
        }
        nodeStack.push(newNode);
        prevIndent = indent;
    }

    file.close();
    return root;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    ASTNode *root = parseASTFromFile(inputFile);
    if (root == nullptr)
    {
        return 1;
    }

    ASTPythonGenerator generator;
    std::string pythonCode;

    for (auto child : root->children)
    {
        if (child->type == "FUNCTION_DEFINITION")
        {
            pythonCode += generator.generatePython(child);
            pythonCode += "\n";
        }
    }

    std::ofstream outputFileStream(outputFile);
    if (!outputFileStream.is_open())
    {
        std::cerr << "Error: Could not open output file " << outputFile << std::endl;
        delete root;
        return 1;
    }

    outputFileStream << pythonCode;
    outputFileStream.close();

    std::cout << "Python code has been successfully written to " << outputFile << std::endl;

    delete root;
    return 0;
}
