# Compiler Design Homeworks (hw1-hw5)

**Course:** NYCU 113 spring CSIC30098 Compiler Design

This repository contains the solutions for the compiler design homework assignments. Each homework builds upon the previous one, progressively creating a compiler for the "P" programming language.

## Homework 1: Lexical Analysis (Scanner)

This homework involved implementing a lexical analyzer (scanner) using Flex. The scanner's purpose is to read the source code of the "P" language and convert it into a stream of tokens. Key responsibilities of the scanner include:
- Recognizing keywords, identifiers, constants (integers, floats, strings), and operators.
- Handling comments and whitespace.
- Detecting and reporting lexical errors, such as invalid characters or malformed tokens.

## Homework 2: Syntax Analysis (Parser)

Building on the scanner from homework 1, this assignment focused on creating a parser using Bison. The parser takes the stream of tokens from the scanner and verifies if the code adheres to the grammatical rules of the "P" language. The key tasks were:
- Defining the grammar of the "P" language using BNF-style rules.
- Implementing an LALR(1) parser to construct a parse tree.
- Handling syntax errors when the input code violates the language's grammar.
- Integrating the scanner to feed tokens to the parser.

## Homework 3: Abstract Syntax Tree (AST) Construction

In this homework, the parser was extended to build an Abstract Syntax Tree (AST) during the parsing process. The AST is a tree representation of the source code that captures its structure in a more abstract form than the parse tree. The main components of this assignment were:
- Designing and implementing a set of C++ classes to represent the nodes of the AST (e.g., for statements, expressions, declarations).
- Modifying the Bison grammar rules to create and link AST nodes.
- Implementing the Visitor design pattern to allow for easy traversal and processing of the AST in later stages.

## Homework 4: Semantic Analysis

This assignment focused on implementing a semantic analyzer. The semantic analyzer traverses the AST to check for semantic errors, which are errors that are not caught by the parser (e.g., type mismatches, undeclared variables). The key features of the semantic analyzer are:
- Implementation of a symbol table to keep track of identifiers (variables, functions, etc.) and their attributes (type, scope).
- Scope management to handle nested scopes (e.g., in functions and blocks).
- Type checking for expressions and assignments.
- Verification of function calls, including checking the number and types of arguments.

## Homework 5: Code Generation (RISC-V)

The final homework was to implement a code generator that translates the AST into RISC-V assembly code. This involved:
- Traversing the AST using the Visitor pattern.
- Generating RISC-V instructions for various language constructs, including:
    - Declarations of global and local variables.
    - Arithmetic and logical expressions.
    - Control flow statements (if-else, for, while).
    - Function calls and returns, including stack frame management.
- Emitting the generated assembly code to a `.S` file.