# MLang
A type-safe programming language for machine learning, optimized for GPU performance using CUDA, is being developed as part of the course COMS 4115: Programming Languages & Translators, Fall 2024, under the guidance of Professor Baishakhi Ray.

# Team Details 
The team members for this project are as follows:

| Name           | UNI    | Email ID                  |
|----------------|--------|---------------------------|
| Alok Mathur    | am6499 | am6499@columbia.edu      |
| Aditi Chowdhuri | anc2207 | anc2207@columbia.edu    |


# Introduction 
The rapid growth in machine learning (ML) and the increasing need for processing large-scale datasets have highlighted the importance of designing programming languages optimized for these tasks. Existing programming languages often fall short in putting to use the full capability of GPU while maintaining type safety and ease of use, particularly for machine learning workflows. To address these challenges, this project proposes the development of a new programming language and compiler specifically designed for machine learning, with a focus on type safety and GPU-based computation using NVIDIA's CUDA architecture.

The proposed language aims to provide a seamless and efficient environment for handling machine learning tasks, particularly for processing multivariate datasets and performing key algorithms that we will be focusing on now is Linear Regression, with Gradient Descent as the optimization method. This language will offer strict type safety, ensuring robust error checking during compilation to prevent runtime issues, especially for matrix and vector operations, which are critical in ML applications.

By leveraging CUDA for parallel execution, the language will enable highly efficient GPU-based computations, making it apt for handling large datasets and complex models. In addition, it will provide built-in functions for machine learning, simplifying common tasks such as dataset handling, model training, and predictions. A key feature will be its ability to handle multivariate datasets with ease, ensuring that users can implement ML models safely and efficiently.

As a proof of concept, the project will focus on implementing Linear Regression, a fundamental algorithm in machine learning. This will demonstrate the language’s ability to process real-world datasets, perform model training using gradient descent, and generate accurate predictions. By combining type safety with GPU acceleration, this project aims to create a powerful, efficient, and user-friendly tool for machine learning development.




# Lexical Analysis Phase

# Lexical Grammar 
### Token Types

| **Token Type** | **Description** |
| -------------- | --------------- |
| `KEYWORD`      | Reserved words of the language |
| `IDENTIFIER`   | Names for variables, functions, etc. |
| `LITERAL`      | Numeric and string constants |
| `OPERATOR`     | Symbols for operations |
| `DELIMITER`    | Punctuation marks for structuring code |
| `COMMENT`      | Single-line comments (not included in final token stream) |
| `UNKNOWN`      | Invalid or unrecognized tokens |
| `END_OF_FILE`  | Marks the end of input |


### Keywords
The following keywords are reserved and case-sensitive:

```
dataset  fn  for  in  return  if  else  while
Int  Float Void Vector  Matrix  to  Dataset
```

### Identifiers
- Start with a letter or underscore
- Can contain letters, digits, and underscores
- Cannot start with a digit
- Cannot be a keyword

### Literals

| **Type**   | **Format**                                 | **Example** |
| ---------- | ------------------------------------------ | ----------- |
| `Integer`  | Sequence of digits                         | 123         |
| `Float`    | Sequence of digits with a single decimal point | 123.45      |

String literals
- Enclosed in double quotes ("")
- Can span multiple lines
- Unterminated strings (missing closing quote) are considered errors

### Operators and Delimiters

| **Type**                     | **Symbols**            |
| ---------------------------- | ---------------------- |
| `Single-character operators`  | `+ - * / = < >`        |
| `Multi-character operators`   | `.. ->`                |
| `Delimiters`                  | `( ) { } [ ] , : ; .`  |


### Comments
Single-line comments: Start with // and continue to the end of the line

### Whitespace
Spaces, tabs, and newlines are ignored except as token separators


# Lexical Rules 
1. The lexer processes the input character by character, identifying tokens.
2. Whitespace is skipped but used to separate tokens.
3. Keywords are checked against a predefined set and take precedence over identifiers.
4. Identifiers and keywords are scanned until a non-alphanumeric, non-underscore character is encountered.
5. Numbers are scanned as a sequence of digits, allowing one decimal point for floats.
6. Strings are scanned between double quotes, allowing for multi-line strings.
7. Operators and delimiters are recognized as single characters or specific two-character sequences.
8. Comments starting with // are skipped and not included in the token stream.
9. The lexer reports errors for:
- Unexpected characters
- Invalid identifiers
- Invalid numbers
- Unterminated strings
10. The END_OF_FILE token is added at the end of the token stream


# State Transitions



# Lexical Errors

The possible lexical errors that are handled are 

| **Error Type**            | **Description**                                        | **Example**                          |
| ------------------------- | ----------------------------------------------------- | ------------------------------------ |
| `Unexpected Character`     | A character that doesn't belong to any valid token    | `@` or any other unrecognized symbol |
| `Unterminated String`      | A string literal without a closing double quote       | `"This string never ends`            |
| `Invalid Number`           | A numeric literal with incorrect format               | `123.45.67` (multiple decimal points)|
| `Invalid Identifier`       | An identifier that doesn't follow the rules           | `123abc` (starts with a digit)       |


### Error Reporting Format
Errors are reported in the following format:
```
filename:line:column: error: [Error message]
```

### Specific error message

1) Unexpected Character
Message: "Unexpected character '[character]'"
Triggered when an unrecognized character is encountered
2) Unterminated String
Message: "Unterminated string literal"
Triggered when a string literal is not closed before the end of the file
3) Invalid Number
Message: "Invalid number '[number]'"
Triggered when a numeric literal doesn't follow the correct format
4) Invalid Identifier
Message: "Invalid identifier '[identifier]'"
Triggered when an identifier doesn't follow the naming rules


### Error Recovery Strategy 
The lexer employs a simple error recovery strategy:
1) Error Detection: When an error is encountered, it is immediately detected and reported.
2) Error Reporting: The error is reported to the standard error stream (stderr) using the format described above. This includes the filename, line number, column number, and a descriptive error message.
3) Continued Lexing: After reporting an error, the lexer continues processing the input. It does not attempt to correct or recover from the error beyond reporting it.
4) Token Generation for Errors: When an error occurs, the lexer generates an UNKNOWN token for the problematic input. This allows the parsing phase to potentially continue, even in the presence of lexical errors.
5) Multiple Error Reporting: The lexer is capable of reporting multiple errors in a single pass. It doesn't stop at the first error encountered.

This strategy allows for:
- Immediate feedback on lexical errors
- The ability to report multiple errors in a single analysis
- Potential continuation of the compilation process, allowing for more comprehensive error reporting in later stages




# Prerequisites for Installation 

1. Installing Git
On Ubuntu/Debian
```
$ sudo apt install git-all
```

2. Installing g++
On Ubuntu/Debian
```
sudo apt update
sudo apt install g++
```
On macOS
```
xcode-select --install
```

# Installation 
1. Clone the Repository 
To clone the repository use the command below
```
git clone https://github.com/alok27a/MLang.git
```


2.Navigating to the folder 

```
cd MLang 
```

3. Running the shell script
This command is executed in the source root directory
```
./run_lexer.sh <example_file.txt>
```
Example 
```
./run_lexer.sh ./mlang_syntax/input/example1.txt
```

There are 5 example text files given in ./mlang_syntax/input/ directory just select any one of the files these are the commands for the same 
```
./run_lexer.sh ./mlang_syntax/input/example1.txt
./run_lexer.sh ./mlang_syntax/input/example2.txt
./run_lexer.sh ./mlang_syntax/input/example3.txt
./run_lexer.sh ./mlang_syntax/input/example4.txt
./run_lexer.sh ./mlang_syntax/input/example5.txt
```

4. Generated Output 
The output is generated in the folder `mlang_syntax/output/output.txt` 


# Lexical Analysis Output Examples 


