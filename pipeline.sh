#!/bin/bash

# Ensure the script stops on errors
set -e

# Define default input file if not provided as an argument
INPUT_FILE=${1:-default_input.txt}

# Update and install necessary packages
echo "Updating system and installing dependencies..."
sudo apt-get update && sudo apt-get install -y build-essential

# Define paths
BASE_DIR=$(pwd)
LEXER_SRC="$BASE_DIR/mlang_compile/src/lexical-analysis/lexer"
ERRORS_SRC="$BASE_DIR/mlang_compile/src/lexical-analysis/errors"
AST_SRC="$BASE_DIR/mlang_compile/src/ast/ast-generation"
CODEGEN_SRC="$BASE_DIR/mlang_compile/src/code-generation"
INPUT_DIR="$BASE_DIR/input"
OUTPUT_DIR="$BASE_DIR/mlang_syntax/code-generation"

# Compile lexer
echo "Compiling Lexer..."
g++ "$LEXER_SRC/main.cpp" "$ERRORS_SRC/errors.cpp" "$LEXER_SRC/lexer.cpp" -o "$BASE_DIR/lexer_program"

# Run lexer
echo "Running Lexer..."
"$BASE_DIR/lexer_program" "$INPUT_DIR/$INPUT_FILE" > "$OUTPUT_DIR/lexer-output/output.txt"

# Compile AST
echo "Compiling AST..."
g++ "$AST_SRC/ast.cpp" -o "$BASE_DIR/ast_program"

# Run AST generation
echo "Running AST generation..."
"$BASE_DIR/ast_program" "$OUTPUT_DIR/lexer-output/output.txt" "$OUTPUT_DIR/ast-output/output.txt"

# Compile Code Generation
echo "Compiling Code Generation..."
g++ "$CODEGEN_SRC/main.cpp" -o "$BASE_DIR/codegen_program"

# Run Code Generation
echo "Running Code Generation..."
"$BASE_DIR/codegen_program" "$OUTPUT_DIR/ast-output/output.txt" "$OUTPUT_DIR/final-output/final_python_output.txt"

# Print final output
echo "Pipeline completed successfully. Final output:"
cat "$OUTPUT_DIR/final-output/final_python_output.txt"
