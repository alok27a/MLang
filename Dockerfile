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
    g++ main.cpp lexical-analysis/errors/errors.cpp lexical-analysis/lexer/lexer.cpp -o mainprogram

# Create a wrapper script to run the program
RUN echo '#!/bin/bash\n\
if [ $# -eq 0 ]; then\n\
    echo "No input file specified. Usage: ./run_lexer.sh <input_filename>"\n\
    exit 1\n\
fi\n\
input_file=$(realpath "/MLANG/mlang_syntax/input/$1")\n\
cd /MLANG/mlang_compile/src\n\
./mainprogram "$input_file" > /MLANG/mlang_syntax/output/output.txt\n\
echo "Execution completed. Output is saved to mlang_syntax/output/output.txt"\n\
rm -f mainprogram' > /MLANG/run_lexer.sh && \
    chmod +x /MLANG/run_lexer.sh

# Set the entrypoint to the wrapper script
ENTRYPOINT ["/MLANG/run_lexer.sh"]

# Default command (can be overridden)
CMD ["example1.txt"]