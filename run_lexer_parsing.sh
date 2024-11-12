#!/bin/bash

# Enable debugging
# set -x

# Check if the input file argument is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input_file>"
    exit 1
fi

# Input file provided as the first argument
input_file=$1

# Print the provided input file path for debugging
# echo "Input file: $input_file"

# Set relative path for the output file
output_file="../../../../mlang_syntax/ast/output/ast_output.txt"

# Print paths for debugging
# echo "Relative path to output file: $output_file"

# Navigate to the directory containing ast.cpp
cd ./mlang_compile/src/ast/ast-generation || { echo "Directory not found"; exit 1; }

# Compile the C++ program
g++ ast.cpp -o ast_program
if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

# Ensure the compiled program is executable
chmod +x ast_program

# Run the compiled program with the adjusted input and output file paths
./ast_program "../../../../$input_file" "$output_file"
if [ $? -ne 0 ]; then
    echo "Execution failed"
    exit 1
fi

echo "AST output has been saved to $output_file"

# Disable debugging
# set +x
