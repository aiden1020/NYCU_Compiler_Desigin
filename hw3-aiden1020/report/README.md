# hw3 report

|      |                 |
| ---: | :-------------- |
| Name | 黃梓濤        |
|   ID | 313552001 |

## How much time did you spend on this project

> e.g. 2 hours.
Total about 24 hours.

for detail, 2 hours for reading readme file and revise the basic concept of OOP and AST, 21 hours for coding, and 1 hour for writing this report.
## Project overview

> Please describe the structure of your code and the ideas behind your implementation in an organized way.

## Overview: AST Construction and Parser Design

This assignment implements a parser for a simple programming language and constructs its corresponding **Abstract Syntax Tree (AST)** using **Bison** (for parsing) and **Flex** (for tokenization). The design leverages the **Visitor Pattern** to allow structured traversal of the AST for later usage (e.g., pretty-printing, semantic checking).

---

## Code Structure

### 1. **Parser (`parser.y`)**

* Defines the grammar rules using BNF-style syntax.
* Each grammar rule constructs an appropriate AST node (e.g., `IfNode`, `BinaryOperatorNode`, `CompoundStatementNode`) and connects children nodes accordingly.
* `%union` defines different semantic value types like `expression_ptr`, `statement_ptr`, etc.
* `%type` maps grammar rules to the expected type.
* `%prec` is used to resolve ambiguity for unary minus.

**Key Grammar Elements:**

| Grammar Rule         | Description                                                      |
| -------------------- | ---------------------------------------------------------------- |
| `Program`            | Root node, contains global declarations and main body            |
| `FunctionDefinition` | Contains name, parameters, return type, and body                 |
| `Statement`          | Unified interface for various statement types                    |
| `Expression`         | Supports constants, variables, arithmetic, and logical operators |

---

### 2. **AST Nodes (in `/include/AST/` and `/lib/AST/`)**

* All AST nodes inherit from `AstNode`.
* Each node stores **source location (line, col)** for debugging and error reporting.
* Some nodes (like `BinaryOperatorNode`, `FunctionNode`, `CompoundStatementNode`) have children and override `visitChildNodes()`.

**Examples:**

| AST Node Class           | Purpose                                                  |
| ------------------------ | -------------------------------------------------------- |
| `FunctionNode`           | Stores function name, arguments, return type, body       |
| `BinaryOperatorNode`     | Represents binary operators like `+`, `-`, `==`, etc     |
| `UnaryOperatorNode`      | Represents unary operations like negation (`-`) or `not` |
| `PrintNode` / `ReadNode` | Represents simple I/O instructions                       |
| `CompoundStatementNode`  | Groups local declarations and statements                 |
| `VariableReferenceNode`  | Represents variables with optional indices (arrays)      |
| `ConstantValueNode`      | Represents literal values of various types               |

---

### 3. **Visitor Pattern**

The Visitor Pattern is for the traversal and processing of the AST in this assignment. Each node in the tree inherits from the base class AstNode, which defines a pure `virtual accept()` method. This method is overridden in each subclass to call the corresponding `visit()` method in the visitor class.

For example, in BinaryOperatorNode, the method `accept(visitor)` calls `visitor.visit(*this)`. This ensures that the correct overloaded function in the visitor is called based on the runtime type of the node.

To traverse the full tree, each node also implements `visitChildNodes()`, where it recursively calls `accept()` on its children. 

The main visitor implemented in this project is AstDumper, which prints out the structure of the tree. When the root node of the AST (i.e., the ProgramNode) is passed into AstDumper, it recursively traverses every child node, indenting the output to reflect the tree hierarchy. This traversal was instrumental in verifying that the AST was constructed correctly and that parent-child relationships were properly maintained.


---

## Implementation Ideas

1. **Modularity and Extensibility**

   * Each language construct has a corresponding AST node.
   * Easy to extend with more node types.


2. **Operator Precedence and Associativity**

   * `%left`, `%right`, `%prec` directives in Bison help resolve ambiguities, especially for unary/binary minus.


> The point is to show us how you deal with the problems. It is not necessary to write a lot of words or paste all of your code here.


A major issue came from type mismatches in Bison’s `%union` and `%type` declarations. Several rules in the grammar needed consistent and explicitly specified value types to avoid default actions from Bison, which led to `type clash` warnings. By refining `%union` definitions and ensuring each grammar rule had a matching `%type`, I resolved these warnings and made the parse tree construction robust.

A particularly subtle bug appeared in the precedence of unary and binary expressions. In a case like `print -1 + 8;`, the parser initially misinterpreted the expression as applying unary minus to the result of `1 + 8`, rather than applying it to `1` first. This was fixed by assigning proper precedence to the unary minus operator using `%prec UMINUS` and correctly structuring the grammar to reflect operator associativity.

The visitor pattern was used to traverse the AST and print out its structure. Each node implements a `visitChildNodes` function that recursively accepts a visitor, which in this case is the `AstDumper`. Debugging traversal involved printing pointer addresses and manually checking which nodes were being visited. Through this process, I discovered that certain nodes were either null or prematurely deleted, causing traversal to fail. Proper memory handling and ensuring all child nodes were correctly constructed and connected resolved these issues.

For complex constructs such as `VariableReferenceNode`, special care was taken to handle multi-dimensional array indices by passing a vector of expression nodes. Initially, I used the wrong type for array subscripts, which led to constructor mismatches. By correcting the grammar to produce a `std::vector<ExpressionNode*>` and adjusting the constructor parameters accordingly, the problem was resolved.

## What is the hardest you think in this project

> Not required, but bonus point may be given.

The hardest parts of implementing this project was correctly defining the grammar rules in Bison to reflect the syntactic structure of the language while ensuring they generated the correct AST node types. Handling operator precedence and associativity proved particularly challenging, especially for expressions involving both unary and binary operators. Without explicitly defining precedence using Bison’s %precedence and %left/%right directives, constructs like -1 + 2 were parsed incorrectly, leading to incorrect ASTs. Another difficulty involved constructing recursive list rules—such as for function arguments or compound statement contents—where careful management of ownership and memory was required to avoid leaking or double-freeing pointers.

## Feedback to T.A.s

> Not required, but bonus point may be given.

* The assignment is well-structured and builds up language features progressively, which helps in managing complexity.
* The provided codebase was clean and easy to follow, especially the AST class structure.
* The debugging process taught me a lot about Bison's behavior, especially how it handles type mismatches and ambiguous grammar.
* It would be nice if the assignment provided some common parser bugs and how to debug them.
