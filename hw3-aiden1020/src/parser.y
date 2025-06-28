/* %debug */
%{
#include "AST/BinaryOperator.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/expression.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/if.hpp"
#include "AST/print.hpp"
#include "AST/program.hpp"
#include "AST/read.hpp"
#include "AST/return.hpp"
#include "AST/variable.hpp"
#include "AST/while.hpp"
#include "AST/AstDumper.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern uint32_t line_num;   /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

static AstNode *root;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

// This guarantees that headers do not conflict when included together.
%define api.token.prefix {TOK_}

%code requires {
    #include <vector>
    #include <iostream>
    class AstNode;              
    class VariableNode;
    class DeclNode;
    class ConstantValueNode;
    class CompoundStatementNode;
    class FunctionNode;
    class BinaryOperatorNode;
    class ExpressionNode;
    class FunctionInvocationNode;
    class AssignmentNode;
    class PrintNode;
    class VariableReferenceNode;
    class IfNode;
    class WhileNode;
    class ForNode;
    class ReturnNode;
}

    /* For yylval */
%union {
    char *identifier;
    int int_val;
    double real_val;
    bool bool_val;
    char *string_val;
    char *keyword;
    char *op;
    AstNode *node;
    CompoundStatementNode *compound_stmt_ptr;
    std::vector<VariableNode*> *var_list_ptr;
    DeclNode *decl_ptr;
    std::vector<DeclNode*> *decl_list_ptr;
    FunctionNode *function_ptr;
    std::vector<FunctionNode*> *function_list_ptr;
    ConstantValueNode *constant_ptr;
    std::vector<AstNode*> *statement_list_ptr;
    AstNode *statement_ptr;
    BinaryOperatorNode *binary_op_ptr;
    std::vector<BinaryOperatorNode> *binary_op_list_ptr;
    ExpressionNode *expression_ptr;
    std::vector<ExpressionNode*> *expression_list_ptr;
    FunctionInvocationNode *function_invocation_ptr;
    VariableReferenceNode *variable_ref_ptr;
    IfNode *if_ptr;
    WhileNode *while_ptr;
    ForNode *for_ptr;
    ReturnNode *return_ptr;
}

%type <var_list_ptr> IdList
%type <compound_stmt_ptr> CompoundStatement ElseOrNot
%type <constant_ptr> LiteralConstant IntegerAndReal StringAndBoolean
%type <decl_ptr> Declaration
%type <decl_list_ptr> Declarations DeclarationList 
%type <decl_ptr> FormalArg
%type <decl_list_ptr> FormalArgList FormalArgs
%type <function_ptr> Function FunctionDeclaration FunctionDefinition
%type <function_list_ptr>Functions FunctionList  
%type <keyword> Type ArrType ArrDecl ScalarType
%type <statement_ptr> Statement Simple
%type <statement_list_ptr> StatementList Statements
%type <expression_ptr>  Expression 
%type <expression_list_ptr> Expressions ExpressionList ArrRefs ArrRefList 
%type <function_invocation_ptr> FunctionInvocation FunctionCall
%type <variable_ref_ptr> VariableReference  
%type <if_ptr> Condition
%type <while_ptr> While
%type <for_ptr> For
%type <return_ptr> Return
%type <identifier> ProgramName FunctionName
%type <identifier> ID
%type <int_val> INT_LITERAL NegOrNot
%type <real_val> REAL_LITERAL
%type <string_val> STRING_LITERAL 
%type <bool_val> TRUE FALSE
%type <keyword> ARRAY BOOLEAN INTEGER REAL STRING END BEGIN DO ELSE FOR IF THEN WHILE DEF OF TO RETURN VAR PRINT READ ReturnType
%type <op>  ASSIGN MULTIPLY DIVIDE MOD LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL OR AND NOT PLUS MINUS     
/* Follow the order in scanner.l */

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
    /* TODO: specify the precedence of the following operators */
%token MULTIPLY DIVIDE MOD
%token LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%token OR AND NOT
%token PLUS MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

    /* Identifier */
%token ID

    /* Literal */
%token INT_LITERAL
%token REAL_LITERAL
%token STRING_LITERAL
%left OR
%left AND
%nonassoc NOT
%nonassoc LESS LESS_OR_EQUAL GREATER GREATER_OR_EQUAL EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UMINUS

%%

ProgramUnit:
    Program
    |
    Function
;

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column,
                               $1,*$3,*$4,$5);
        delete $3;
        delete $4;
        free($1);
    }
;

ProgramName:
    ID
;

DeclarationList:
    Epsilon {
        $$ = new std::vector<DeclNode*>();
    }
  | Declarations
;

Declarations:
    Declaration {
        $$ = new std::vector<DeclNode*>();
        $$->push_back($1);
    }
  | Declarations Declaration {
        $1->push_back($2);
        $$ = $1;
    }
;

FunctionList:
    Epsilon{
        $$ = new std::vector<FunctionNode*>();
    }
    |
    Functions
;

Functions:
    Function {
        $$ = new std::vector<FunctionNode*>();
        $$->push_back($1);
    }
    |
    Functions Function {
        $1->push_back($2);
        $$ = $1;
    }
;

Function:
    FunctionDeclaration
    |
    FunctionDefinition
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON {
        $$ = new FunctionNode(@1.first_line, @1.first_column,
                              $1, *$3, $5, nullptr);  
    }

;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType CompoundStatement END {
        $$ = new FunctionNode(@1.first_line, @1.first_column,
                              $1, *$3, $5, $6);
    }
;

FunctionName:
    ID
;

FormalArgList:
    Epsilon {
        $$ = new std::vector<DeclNode*>();
    }
    |
    FormalArgs
;

FormalArgs:
    FormalArg{
        $$ = new std::vector<DeclNode*>();
        $$->push_back($1);
    }
    |
    FormalArgs SEMICOLON FormalArg {
        $1->push_back($3);
        $$ = $1;
    }
;

FormalArg:
    IdList COLON Type {
        for (auto var : *$1)
            var->setType($3);

        $$ = new DeclNode(@1.first_line, @1.first_column, *$1, $3);
    }
;

IdList:
    ID{
        auto var = new VariableNode(@1.first_line, @1.first_column, std::string($1), "", nullptr);
        auto vec = new std::vector<VariableNode*>();
        vec->push_back(var);
        $$ = vec;
    }
    |
    IdList COMMA ID{
        auto var = new VariableNode(@3.first_line, @3.first_column, std::string($3), "", nullptr);
        $1->push_back(var);
        $$ = $1;
    }
;

ReturnType:
    COLON ScalarType{
        $$ = strdup($2);
        free($2);
    }
    |
    Epsilon{
        $$ = strdup("void");
    }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON {
        for (auto var : *$2)
            var->setType($4);

        $$ = new DeclNode(@1.first_line, @1.first_column, *$2 , $4);

    }
  |
    VAR IdList COLON LiteralConstant SEMICOLON {
        for (auto var : *$2) {
            var->setType($4->getTypeCString());
            var->setConstantValue($4);
        }

        $$ = new DeclNode(@1.first_line, @1.first_column, *$2 , $4);
    }
;

Type:
    ScalarType
    |
    ArrType
;

ScalarType:
    INTEGER
    |
    REAL 
    |
    STRING
    |
    BOOLEAN
;

ArrType:
    ArrDecl ScalarType{
        std::string arr_str($1);
        std::string type_str($2);
        type_str += " " + arr_str;
        $$ = strdup(type_str.c_str());

    }
;

ArrDecl:
    ARRAY INT_LITERAL OF{
        std::string arr_str = "[" + std::to_string($2) + "]";
        $$ = strdup(arr_str.c_str());  
    }
    |
    ArrDecl ARRAY INT_LITERAL OF {
        std::string arr_str($1);
        arr_str += "[" + std::to_string($3)+ "]";
        $$ = strdup(arr_str.c_str());
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL {
        if ($1 == 1){
            $$ = new ConstantValueNode(@2.first_line, @2.first_column, $2);
        }
        else {
            $$ = new ConstantValueNode(@1.first_line, @1.first_column, -$2);
        }
    }
  | NegOrNot REAL_LITERAL {
        if ($1 == 1){
            $$ = new ConstantValueNode(@2.first_line, @2.first_column, $2);
        }
        else {
            $$ = new ConstantValueNode(@1.first_line, @1.first_column, -$2);
        }
    }
  | TRUE {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, true);
    }
  | FALSE {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, false);
    }
  | STRING_LITERAL {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, std::string($1));
    }
;

NegOrNot:
    Epsilon{ $$ = 1; }  
    |
    MINUS{ $$ = -1; };
;

StringAndBoolean:
    STRING_LITERAL {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, std::string($1));
    }
  | TRUE{
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1); 
    }
  | FALSE{
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1); 
    }
;

IntegerAndReal:
    INT_LITERAL{
            $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1); 
    }
    |
    REAL_LITERAL{
            $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1); 
    }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement{
        $$ = $1;
    }
    |
    Simple
    |
    Condition{
        $$ = $1;
    }
    |
    While{
        $$ = $1;
    }
    |
    For{
        $$ = $1;
    }
    |
    Return{
        $$ = $1;
    }
    |
    FunctionCall{
        $$ = $1;
    }
;

CompoundStatement:
    BEGIN
    DeclarationList
    StatementList
    END {
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column,
                                       *$2, *$3);
        
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON{
        $$ = new AssignmentNode(@1.first_line, @2.first_column,
                               $1, $3);
    }
    |
    PRINT Expression SEMICOLON{
        $$ = new PrintNode(@1.first_line, @1.first_column, $2);
    }
    |
    READ VariableReference SEMICOLON{
        $$ = new ReadNode(@1.first_line, @1.first_column, $2);
    }
;

VariableReference:
    ID ArrRefList {
        $$ = new VariableReferenceNode(@1.first_line, @1.first_column, std::string($1),*$2);        
    }
;

ArrRefList:
    Epsilon{
        $$ = new std::vector<ExpressionNode*>();
    }
    |
    ArrRefs
;

ArrRefs:
    L_BRACKET Expression R_BRACKET{
        $$ = new std::vector<ExpressionNode*>();
        $$->push_back($2);
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET{
        $1->push_back($3);
        $$ = $1;
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF{
        $$ = new IfNode(@1.first_line, @1.first_column,
                        $2, $4, $5);
    }
;

ElseOrNot:
    ELSE
    CompoundStatement{
        $$ = $2;
    }
    |
    Epsilon{
        $$ = nullptr;
    }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO{
        $$ = new WhileNode(@1.first_line, @1.first_column,
                           $2, $4);
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO{
        auto var = new VariableNode(@1.first_line, @2.first_column,
                            std::string($2), "", nullptr);
        var->setType("integer");
        auto decl = new DeclNode(@1.first_line, @2.first_column,
                              std::vector<VariableNode*>{var},"integer");
        auto var_ref = new VariableReferenceNode(@1.first_line, @2.first_column,
                                        std::string($2), std::vector<ExpressionNode*>());
        auto assign = new AssignmentNode(@1.first_line, @3.first_column,
                                    var_ref, new ConstantValueNode(@1.first_line, @4.first_column, $4));
        auto constant = new ConstantValueNode(@1.first_line, @6.first_column, $6);
        $$ = new ForNode(@1.first_line, @1.first_column,
                         decl, assign, constant, $8);
    }
;

Return:
    RETURN Expression SEMICOLON{
        $$ = new ReturnNode(@1.first_line, @1.first_column, $2);
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON{
        $$ = $1;
    }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS{
        $$ = new FunctionInvocationNode(@1.first_line, @1.first_column,
                                        std::string($1), *$3);
    }
;

ExpressionList:
    Epsilon{
        $$ = new std::vector<ExpressionNode*>();
    }
    |
    Expressions
;

Expressions:
    Expression {
        $$ = new std::vector<ExpressionNode*>();
        $$->push_back($1);
    }
    |
    Expressions COMMA Expression {
        $1->push_back($3);
        $$ = $1;
    }
;

StatementList:
    Epsilon{
        $$ = new std::vector<AstNode*>();
    }
    |
    Statements
;

Statements:
    Statement{
        $$ = new std::vector<AstNode*>();
        $$->push_back($1);
    }
    |
    Statements Statement{
        $1->push_back($2);
        $$ = $1;
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS {
        $$ = $2;
    }
    |
    MINUS Expression %prec UMINUS{
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, $1, $2);
    }
    |
    Expression MULTIPLY Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    Expression DIVIDE Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    Expression MOD Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    Expression PLUS Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    Expression MINUS Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    Expression LESS Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    Expression LESS_OR_EQUAL Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    Expression GREATER Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    Expression GREATER_OR_EQUAL Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    Expression EQUAL Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    Expression NOT_EQUAL Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    NOT Expression {
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, $1, $2);
    }
    |
    Expression AND Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    Expression OR Expression {
        $$ = new BinaryOperatorNode(@1.first_line, @2.first_column, $2, $1, $3);
    }
    |
    IntegerAndReal{
        $$ = $1;
    }
    |
    StringAndBoolean{
        $$ = $1;
    }
    |
    VariableReference{
        $$ = $1;
    }
    |
    FunctionInvocation{
        $$ = $1;
    }
;

    /*
       misc
            */
Epsilon:
;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, current_line, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    // extern int yydebug;
    // yydebug = 1;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [--dump-ast]\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        AstDumper ast_dumper;
        root->accept(ast_dumper);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
