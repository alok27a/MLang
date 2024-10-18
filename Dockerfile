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
WORKDIR /app

# Copy the entire project directory into the container
COPY . .

# Make the shell script executable
RUN chmod +x run_lexer.sh

# Default command to run the shell script
CMD ["./run_lexer.sh"]
