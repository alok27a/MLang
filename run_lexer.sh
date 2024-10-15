#!/bin/bash

# Move to the source directory
cd mlang_compile/src

# Compile the lexer and related files
g++ main.cpp errors.cpp lexer.cpp -o mainprogram

# Check if the compilation succeeded
if [ $? -eq 0 ]; then
    echo "Compilation successful."
    
    # Run the program and redirect output to output.txt
    ./mainprogram > output.txt
    echo "Execution completed. Output is saved to output.txt"
else
    echo "Compilation failed."
    exit 1
fi
