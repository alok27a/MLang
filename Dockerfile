# Use an official Ubuntu base image
FROM ubuntu:20.04

# Set non-interactive mode for installing packages
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary dependencies (build tools, g++)
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    && apt-get clean

# Set the working directory in the container
WORKDIR /MLANG

# Copy the entire project directory into the container
COPY . .

# Compile the lexer and related files
RUN cd mlang_compile/src && \
    g++ main.cpp lexical-analysis/errors/errors.cpp lexical-analysis/lexer/lexer.cpp -o lexer_program

# Compile the AST generation program (assuming ast.cpp is located in 'ast' folder)
RUN cd mlang_compile/src/ast/ast-generation && \
    g++ ast.cpp -o ast_program

# Create the lexer wrapper script
RUN echo '#!/bin/bash\n\
if [ $# -lt 1 ]; then\n\
    echo "No input file specified. Usage: docker run <image_name> run_lexer.sh <input_file>"\n\
    exit 1\n\
fi\n\
input_file=$1\n\
if [ ! -f "$input_file" ]; then\n\
    echo "Input file not found at $input_file"\n\
    exit 1\n\
fi\n\
output_file=${2:-"/MLANG/mlang_syntax/lexer/output/lexer_output.txt"}\n\
cd /MLANG/mlang_compile/src && \\\n\
./lexer_program "$input_file" > "$output_file"\n\
echo "Lexer output is saved to $output_file"\n' > /MLANG/run_lexer.sh && \
    chmod +x /MLANG/run_lexer.sh

# Create the AST generation wrapper script
RUN echo '#!/bin/bash\n\
if [ $# -lt 1 ]; then\n\
    echo "No input file specified. Usage: docker run <image_name> run_ast.sh <input_file>"\n\
    exit 1\n\
fi\n\
input_file=$1\n\
if [ ! -f "$input_file" ]; then\n\
    echo "Input file not found at $input_file"\n\
    exit 1\n\
fi\n\
output_file=${2:-"/MLANG/mlang_syntax/ast/output/ast_output.txt"}\n\
cd /MLANG/mlang_compile/src/ast/ast-generation && \\\n\
./ast_program "$input_file" "$output_file"\n\
echo "AST generation completed. Output saved to $output_file"\n' > /MLANG/run_ast.sh && \
    chmod +x /MLANG/run_ast.sh

# Set the entrypoint to /bin/bash to allow for interactive use
ENTRYPOINT ["/bin/bash"]

# Default command (can be overridden)
CMD ["-c", "echo 'Docker image is ready for interactive use.'"]
