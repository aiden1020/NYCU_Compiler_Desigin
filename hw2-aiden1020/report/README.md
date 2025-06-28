# hw2 report

|||
|-:|:-|
|Name|黃梓濤|
|ID|313552001|

## How much time did you spend on this project

Total about 6 hours.

for detail, 1 hours for reading readme file and revise the basic concept of parser, 4 hours for coding, and 1 hour for writing this report.

## Project overview

> Please describe the structure of your code and the ideas behind your implementation in an organized way. 
### Overview 
In HW2, the LALR(1) parser was implemented to analyze the tokenized input from the scanner and verify the program's structure. It checks syntax based on grammar rules, handling declarations, functions, statements, and expressions. For the scanner was also enable to pass tokens to the parser for efficient syntax analysis.

### Parser Part
#### **1. Program Unit**  
- The program unit consists of either a program or a function.  

#### **2. Program**  
- Begins with an identifier followed by a semicolon.  
- Contains declarations, functions, and a compound statement.  
- Ends with a designated `END_` keyword.  

#### **3. Declarations**  
- Divided into variable declarations and constant declarations.  
- Variable declarations specify identifiers and their types.  
- Constant declarations assign fixed values to identifiers.  

#### **4. Data Types**  
- Consist of scalar types and array types.  
- Scalar types include integers, real numbers, strings, and boolean values.  
- Arrays are defined with a size and a corresponding type.  

#### **5. Functions**  
- Functions can be either declarations or definitions.  
- A function declaration only specifies the header without implementation.  
- A function definition includes a compound statement as its body.  
- Function headers define the function name, parameters, and return type.  

#### **6. Statements**  
- Statements include simple statements, conditional statements, function calls, return statements, and loop statements.  
- Simple statements handle input, output, and assignments.  
- Conditional statements implement `IF-THEN-ELSE` logic.  
- Function call statements allow invoking defined functions.  
- Return statements allow functions to return computed values.  
- Loop statements include `WHILE` and `FOR` loops.  

#### **7. Expressions**  
- Expressions consist of literals, variables, function calls, and operations.  
- Binary operations include arithmetic, logical, and comparison operators.  
- Unary operations include negation and logical NOT.  
- Expressions can be grouped using parentheses to enforce precedence.  

#### **8. Literals**  
- Integer literals include decimal and octal representations.  
- Real literals include floating-point and scientific notation.  
- String literals represent sequences of characters.  
- Boolean literals consist of `TRUE_` and `FALSE_`.  

### Scanner Part
- In HW1, I implemented the scanner, including:
    - Reads Input Source Code: Scans the source code character by character.

    - Token Identification: Recognizes fundamental language components.

    - Token Classification: Categorizes tokens into keywords, identifiers, numbers, operators, etc.

    - Whitespace and Comment Filtering: Ignores spaces, newlines, and comments to prevent interference with syntax analysis.

    - Error Detection: Identifies invalid characters or incorrect tokens and generates error messages.
- In HW2, I extended the functionality by enabling the scanner to transmit the identified tokens to the parser, allowing for further syntactic analysis and program structure validation.

> The point is to show us how you deal with the problems. It is not necessary to write a lot of words or paste all of your code here. 


- When a grammar contains optional elements, such as function parameters or variable declarations, it can cause parsing conflicts. The parser might struggle to decide whether to continue parsing or move on if the optional elements are missing. Without clear instructions, the parser may misinterpret the input.

    To solve this, use `/* empty */` to explicitly define optional elements in the grammar. This tells the parser that some parts may be absent, allowing it to handle missing elements correctly. Ensuring that optional rules are well-defined prevents parsing errors and allows the parser to process both present and absent elements properly.

- A token mismatch occurs when the scanner outputs tokens that the parser does not expect. This happens if the scanner doesn’t identify tokens correctly or if there’s a discrepancy in token definitions between the scanner and the parser.

    To fix this, ensure that the token definitions in the scanner match those expected by the parser. Verify that all tokens used in the parser are correctly defined in the scanner. Debugging the scanner by printing tokens will help ensure that it is producing the correct tokens, enabling the parser to parse the input correctly.
## What is the hardest you think in this project

In this project, the hardest challenge was handling parsing conflicts, especially in the LALR(1) parser. Here's how I addressed some common issues:

1. **Optional Elements:** I used `/* empty */` to handle optional parts like empty function arguments or declarations, making sure the parser could process these cases without errors.

2. **Left Recursion:** I avoided left recursion by rewriting rules like `declaration_list` and `function_list` to use right recursion. This helped prevent conflicts in the parsing process.

3. **Function Arguments:** I designed the `formal_argument_list` to handle empty arguments, allowing the parser to process function headers correctly when no arguments were present.

4. **Statement Lists:** By allowing `statement_list` to be empty, I made sure the parser could handle cases with no statements and still parse other parts of the code correctly.

5. **Keyword and Identifier Confusion:** I carefully structured conditional statements to avoid ambiguity between keywords (like `IF`, `THEN`, `ELSE`) and identifiers, ensuring the parser could distinguish between them.


## Feedback to T.A.s

- The guidance on parser construction was useful, especially with handling parsing conflicts.

- The explanations on managing optional elements, recursion issues, and syntax errors were very helpful.