# hw4 report

|||
|-:|:-|
|Name|黃梓濤|
|ID|313552001|

## How much time did you spend on this project

> e.g. 2 hours.
Total about 20 hours.

More specifically, 2 hours were spent reading the README file and reviewing the basic concepts of semantic analysis, 17 hours were spent on coding, and 1 hour on writing this report.
## Project overview

> Please describe the structure of your code and the ideas behind your implementation in an organized way.
> The point is to show us how you deal with the problems. It is not necessary to write a lot of words or paste all of your code here.
### Overview

In this assignment, semantic analysis was performed on the P language by extending the existing compiler infrastructure. A robust symbol table supporting scope management, identifier storage, and redeclaration checking was implemented. Comprehensive semantic checks covered dimension correctness, identifier usage, type compatibility for operations, function parameter matching, and both scalar and array operations. The Abstract Syntax Tree (AST) was enhanced with semantic attributes for type inference and validation. Semantic errors were precisely reported with clear diagnostics. 
## Code Structure

### Abstract Syntax Tree (AST)

* The AST was extended with semantic attributes, such as inferred types and dimensions, for each node. This enables semantic validation and type checking during traversal, facilitating early detection of semantic errors.

### Semantic Analyzer

* **SemanticAnalyzer:** Implemented as a visitor pattern to traverse AST nodes and apply semantic rules, including type checking and scope validation.
* **SymbolEntry:** Stores detailed information about each symbol, including names, types, scope levels, and attributes such as array dimensions and constant values.
* **SymbolTable:**  Manages symbols within a single scope, supporting insertion, redeclaration checking, and retrieval of symbol information.
* **SymbolManager:** Maintains multiple symbol tables for different nested scopes using a stack-based approach, ensuring symbols are accessible only within their respective scopes.

## Implement detail and problems
During the implementation of the semantic analyzer, several challenges were encountered:

1. Managing nested scopes with proper symbol visibility required careful design of the symbol table and symbol manager. Mistakes in scope entry and exit could lead to incorrect lookups or symbol redeclarations.
A symbol table structure with parent-child relationships was implemented, and the symbol manager was designed to push a new table upon entering each new scope. Lookups were handled by searching the current scope first, then recursively searching parent tables to ensure correct shadowing and visibility. When exiting a block, the appropriate table was popped, maintaining accurate visibility and preventing redeclaration errors. Special care was taken with loops and function parameters by adjusting the symbol kind as needed to avoid misclassification or redeclaration issues.

2. Implementing precise type inference for array references was non-trivial; correctly reducing dimensions based on indices used in the AST required detailed logic to reflect scalar or sliced types accurately.
For array references, type inference was achieved by retrieving the base type and dimensions from the symbol table, then adjusting the type according to the number of indices used. If all dimensions were indexed, a scalar type was returned; if some remained, a new array type with the remaining dimensions was constructed. Integer type checks on indices were enforced before inference, with errors reported immediately for mismatches. In this way, distinctions between scalars and arrays were properly reflected in the type system.

3. Distinguishing between different operand combinations for binary operators (e.g., supporting both numeric and string addition but disallowing string subtraction) involved writing detailed and sometimes complex type-checking conditions.
Explicit type-checking rules for each binary operator were written to enforce valid operand combinations and report precise errors. The `+` operator was supported for both numeric and string types, while `-`, `*`, and `/` were restricted to numeric types, and logical operators like `and` and `or` were restricted to boolean types. Relational operators were checked for compatibility among scalar types or strings as appropriate. When invalid combinations were encountered, informative error messages were generated, enabling strict adherence to language semantics and easier debugging.

4. An early design mistake was made in the SymbolEntry class, where a `std::string m_attribute` field was incorrectly used to store function attributes such as parameter types. This approach introduced complexity when retrieving and checking function signatures. To resolve this issue, a helper function `stringToPType` was used in `SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation)` to convert the string-formatted parameter types back into usable type representations for semantic checks.
## What is the hardest you think in this project

> Not required, but bonus point may be given.
> 
The most challenging aspect of this project was ensuring that the semantic analysis accurately reflected the language’s rules, especially in handling complex cases such as nested scopes, type inference for arrays, and operator type checking. Managing scope transitions required precise control over symbol tables to maintain correct visibility and prevent redeclaration errors. Meanwhile, inferring types for array references and verifying operand compatibility in expressions demanded detailed logic and careful error reporting. Balancing all these requirements while maintaining robust and readable code proved to be the most difficult part of the implementation.
## Feedback to T.A.s

> Not required, but bonus point may be given.

* The assignment is well-structured and builds up language features progressively, which helps in managing complexity.
* The provided codebase was clean and easy to follow, especially the AST class structure.
* The debugging process taught me a lot about the behavior of semantic analysis and symbol table management, especially how type inference and scope resolution can lead to subtle bugs.

* It would be helpful if the assignment provided some common semantic analysis pitfalls and strategies for debugging them.
