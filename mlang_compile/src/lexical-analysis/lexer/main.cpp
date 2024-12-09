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

public:
    std::string generatePython(ASTNode *node)
    {
        if (!node)
            return "";
        std::ostringstream python;

        if (node->type == "FUNCTION_DEFINITION")
        {
            python << generateFunctionDefinition(node);
            indentLevel = 0; // Reset indent level after each function
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
                // Convert return type to lowercase
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
                        // Convert parameter type to lowercase
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
        std::cerr << "Generating Python code for FOR_LOOP\n";

        // Add logging to inspect the AST structure
        std::cerr << "Inspecting ASTNode children for FOR_LOOP\n";
        for (auto child : node->children)
        {
            std::cerr << "Child type: " << child->type << " | Child value: " << child->value << "\n";

            if (child->type == "LOOP_VARIABLE")
            {
                std::string fullVar = child->value;
                size_t pos = fullVar.find(" (TYPE:");
                loopVar = (pos != std::string::npos) ? fullVar.substr(0, pos) : fullVar;
                std::cerr << "Found LOOP_VARIABLE: " << loopVar << "\n";
            }
            else if (child->type == "RANGE_START")
            {
                // Check if RANGE_START is a literal value directly
                if (child->value.find("LITERAL_VALUE") != std::string::npos)
                {
                    rangeStart = child->value.substr(child->value.find(":") + 1); // Extract value after "LITERAL_VALUE: "
                    std::cerr << "RANGE_START LITERAL_VALUE: " << rangeStart << "\n";
                }
                else if (!child->children.empty())
                {
                    rangeStart = child->children[0]->value;
                }
                std::cerr << "Found RANGE_START: " << rangeStart << "\n";
            }
            else if (child->type == "RANGE_END")
            {
                // Check if RANGE_END is a literal value directly
                if (child->value.find("LITERAL_VALUE") != std::string::npos)
                {
                    rangeEnd = child->value.substr(child->value.find(":") + 1); // Extract value after "LITERAL_VALUE: "
                    std::cerr << "RANGE_END LITERAL_VALUE: " << rangeEnd << "\n";
                }
                else if (!child->children.empty())
                {
                    rangeEnd = child->children[0]->value;
                }
                std::cerr << "Found RANGE_END: " << rangeEnd << "\n";
            }
        }

        // Add logging to check if loopVar, rangeStart, and rangeEnd are correctly set
        std::cerr << "Loop variable: " << loopVar << "\n";
        std::cerr << "Range start: " << rangeStart << "\n";
        std::cerr << "Range end: " << rangeEnd << "\n";

        if (loopVar.empty() || rangeStart.empty() || rangeEnd.empty())
        {
            std::cerr << "Error: Missing loop variable or range in FOR_LOOP\n";
            return "";
        }

        python << getIndent() << "for " << loopVar << " in range(" << rangeStart << ", " << rangeEnd << "):\n";
        indentLevel++;

        bool foundLoopBody = false;
        for (auto child : node->children)
        {
            if (child->type == "LOOP_BODY")
            {
                foundLoopBody = true;
                std::cerr << "Found LOOP_BODY. It has " << child->children.size() << " children.\n";
                for (auto loopBodyChild : child->children)
                {
                    std::cerr << "Processing LOOP_BODY child of type: " << loopBodyChild->type << "\n";
                    if (loopBodyChild->type == "ASSIGNMENT_EXPRESSION")
                    {
                        std::cerr << "Found ASSIGNMENT_EXPRESSION\n";
                        python << generateAssignment(loopBodyChild);
                    }
                    else if (loopBodyChild->type == "FUNCTION_BODY")
                    {
                        std::cerr << "Found FUNCTION_BODY in LOOP_BODY\n";
                        for (auto funcChild : loopBodyChild->children)
                        {
                            python << generatePython(funcChild);
                        }
                    }
                    else if (loopBodyChild->type == "FOR_LOOP")
                    {
                        std::cerr << "Found nested FOR_LOOP\n";
                        python << generateForLoop(loopBodyChild);
                    }
                }
            }
        }

        if (!foundLoopBody)
        {
            std::cerr << "Error: No LOOP_BODY found in FOR_LOOP\n";
        }

        indentLevel--;
        // std::cerr << "Generated Python code for FOR_LOOP:\n" << python.str() << "\n";
        return python.str();
    }

    std::string generateExpression(ASTNode *node)
    {
        std::ostringstream expression;
        if (node->type == "EXPRESSION")
        {
            // If it's an EXPRESSION node, process its children
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
                expression << left << " " << node->value << " " << right;
            }
            else
            {
                std::cerr << "Error: OPERATOR node does not have 2 children.\n";
            }
        }
        else if (node->type == "LITERAL_VALUE" || node->type == "IDENTIFIER")
        {
            expression << node->value;
        }
        else
        {
            std::cerr << "Unhandled expression type: " << node->type << "\n";
        }
        return expression.str();
    }

    std::string generateAssignment(ASTNode *node)
    {
        std::ostringstream python;
        std::cerr << "Processing ASSIGNMENT_EXPRESSION\n";
        std::string variable;
        std::string value;

        for (auto child : node->children)
        {
            std::cerr << "Child type: " << child->type << ", value: " << child->value << "\n";
            if (child->type == "IDENTIFIER")
            {
                variable = child->value;
                std::cerr << "Found IDENTIFIER: " << variable << "\n";
            }
            if (child->type == "EXPRESSION")
            {
                value = generateExpression(child);
                std::cerr << "Generated EXPRESSION: " << value << "\n";
            }
        }

        if (!variable.empty() && !value.empty())
        {
            python << getIndent() << variable << " = " << value << "\n";
        }
        else
        {
            std::cerr << "Error: Missing variable or value in ASSIGNMENT_EXPRESSION\n";
        }

        return python.str();
    }
    std::string generateReturnStatement(ASTNode *node)
    {
        std::ostringstream python;
        python << getIndent() << "return " << generateExpression(node->children[0]) << "\n";
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
    std::string simplifyExpression(const std::string &expr)
    {
        if (expr == "0 + 0")
            return "0";
        if (expr == "a + 0" || expr == "0 + a")
            return "a";
        if (expr == "a * 1" || expr == "1 * a")
            return "a";
        if (expr == "a * 0" || expr == "0 * a")
            return "0";
        return expr; // return the original if no simplification is possible
    }

    // Function to perform basic simplification of expressions in statements
    void simplifyStatements(std::vector<std::string> &statements)
    {
        for (auto &stmt : statements)
        {
            // If the statement is an assignment, simplify the expression
            if (stmt.find("=") != std::string::npos)
            {
                size_t eqPos = stmt.find("=");
                std::string left = stmt.substr(0, eqPos);
                std::string right = stmt.substr(eqPos + 2);
                stmt = left + " = " + simplifyExpression(right); // Simplify and reassign the statement
            }
        }
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

    // Step 1: Parse the AST from the input file
    ASTNode *root = parseASTFromFile(inputFile);
    if (root == nullptr)
    {
        return 1;
    }

    // Step 2: Generate Python code
    ASTPythonGenerator generator;
    std::string pythonCode;

    for (auto child : root->children)
    {
        if (child->type == "FUNCTION_DEFINITION")
        {
            pythonCode += generator.generatePython(child);
            pythonCode += "\n"; // Add a newline between functions
        }
    }

    // Step 3: Write the Python code to the output file
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