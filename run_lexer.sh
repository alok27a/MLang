#!/bin/bash

# Check if the filename is passed as an argument
if [ $# -eq 0 ]; then
    echo "No input file specified. Usage: ./run_lexer.sh <input_filename>"
    exit 1
fi

# Store the filename from the argument and convert it to an absolute path
input_file=$(realpath $1)

# Move to the source directory
cd mlang_compile/src

# Compile the lexer and related files
g++ main.cpp lexical-analysis/errors/errors.cpp lexical-analysis/lexer/lexer.cpp -o mainprogram

# Check if the compilation succeeded
if [ $? -eq 0 ]; then
    echo "Compilation successful."
    
    # Create the output directory if it doesn't exist
    mkdir -p ../../mlang_syntax/output

    # Run the program and save the output to mlang_syntax/output/output.txt
    ./mainprogram "$input_file" > ../../mlang_syntax/output/output.txt
    echo "Execution completed. Output is saved to mlang_syntax/output/output.txt"
else
    echo "Compilation failed."
    exit 1
fi
