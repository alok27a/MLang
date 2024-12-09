FROM ubuntu:22.04

# Set environment variables to ensure non-interactive apt-get installation
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages (g++, make, git, bash, etc.)
RUN apt-get update && \
    apt-get install -y build-essential && \
    apt-get clean

# Copy all the necessary files to the container (including your script and C++ source files)
COPY . /app

# Set the working directory
WORKDIR /app

# Create a shell script to run the pipeline
RUN echo '#!/bin/bash\n\
INPUT_FILE=${1:-default_input.txt}\n\
g++ /app/mlang_compile/src/lexical-analysis/lexer/main.cpp \
     /app/mlang_compile/src/lexical-analysis/errors/errors.cpp \
     /app/mlang_compile/src/lexical-analysis/lexer/lexer.cpp -o /app/lexer_program\n\
/app/lexer_program /app/input/${INPUT_FILE} > /app/mlang_syntax/code-generation/lexer-output/output.txt\n\
echo "Compiling AST..."\n\
g++ /app/mlang_compile/src/ast/ast-generation/ast.cpp -o /app/ast_program\n\
/app/ast_program /app/mlang_syntax/code-generation/lexer-output/output.txt /app/mlang_syntax/code-generation/ast-output/output.txt\n\
echo "Compiling Code-Generation..."\n\
g++ /app/mlang_compile/src/code-generation/main.cpp -o /app/codegen_program\n\
/app/codegen_program /app/mlang_syntax/code-generation/ast-output/output.txt /app/mlang_syntax/code-generation/final-output/final_python_output.txt\n\
echo "Pipeline completed successfully. Final output is in /app/mlang_syntax/code-generation/final-output/final_python_output.txt"\n\
echo "Contents of final_python_output.txt:"\n\
cat /app/mlang_syntax/code-generation/final-output/final_python_output.txt\n\
' > /app/run_pipeline.sh && chmod +x /app/run_pipeline.sh

# Set the entrypoint to the shell script
ENTRYPOINT ["/app/run_pipeline.sh"]