%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern int32_t line_num;    /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

extern int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%token ',' ';' ':' '(' ')' '{' '}' '[' ']'
%left "+" "-" "*" "/" "%" 
%left LT LE NEQ GE GT EQ
%token AND OR NOT
%token VAR ARRAY OF BOOLEAN INTEGER OCT_INTEGER  REAL FLOAT  SCIENTIFIC STRING STRING_LITERAL ASSIGN
%token TRUE_ FALSE_ DEF RETURN BEGIN_ END_ WHILE DO
%token IF THEN ELSE FOR TO PRINT READ
%token ID  

%%

program_unit: program |function;

/* program */
program: ID ';' declaration_list function_list compound_statement END_;  
declaration_list : /* empty */ | declaration_list1 ;
declaration_list1 : declaration | declaration_list1 declaration ;
function_list : /* empty */ | function_list1 ;
function_list1 : function | function_list1 function ;
compound_statement: BEGIN_  declaration_list statement_list END_;

/* Declarations */
declaration: variable_declaration| constant_declaration ;
variable_declaration:VAR identifier_list ':' type ';';
constant_declaration:VAR identifier_list ':' literal_constant ';';
/* identifier list*/
identifier_list :ID| ID ',' identifier_list ;
/* types */
type: scalar_type | array_type ;
scalar_type: INTEGER | REAL  | STRING |  BOOLEAN ;
array_type: ARRAY INTEGER  OF type | ARRAY OCT_INTEGER  OF type

/* Function */
function: function_declaration | function_defination ;
function_declaration: function_header ';' ;
function_defination : function_header compound_statement END_ ;
function_header: ID '(' formal_argument_list ')' ':' scalar_type| ID '(' formal_argument_list ')' ;
formal_argument_list : /* empty */ | formal_argument_list1 ;
formal_argument_list1 : formal_argument | formal_argument_list1 ';' formal_argument ;
formal_argument: identifier_list ':' type ;


/* statement */
statement_list : /* empty */ | statement_list1 ;

statement_list1 : statement | statement_list1 statement ;

statement: simple_statement |conditional_statement |function_call_statement | return_statement|compound_statement|loop_statement ;
simple_statement: PRINT expression ';' | READ variable_reference ';' | variable_reference ASSIGN expression ';';
conditional_statement : IF expression THEN compound_statement ELSE compound_statement END_ IF |IF expression THEN compound_statement END_ IF;
function_call_statement: function_call ';' ;
loop_statement: WHILE expression DO compound_statement END_ DO| FOR ID ASSIGN integer_literal TO integer_literal DO compound_statement END_ DO;
return_statement: RETURN expression ';' ;

/* expressions */
expression_list: expression | expression ',' expression_list ;
expression: literal_constant | operator_expression | variable_reference |function_call;
literal_constant: integer_literal | real_literal | string_literal | boolean_literal ;
variable_reference: ID | ID '[' expression ']'  ;
function_call :  ID '(' expression_list ')'| ID '(' ')' ;
operator_expression: expression binary_operator expression |  unary_operator expression |'(' expression ')' ;


binary_operator : '+' | '-' | '*' | '/' | '%'| AND | OR | LT | LE | NEQ | GE | GT | EQ;
unary_operator : '-' |NOT ;

/* literals */
integer_literal: INTEGER | '-' INTEGER |OCT_INTEGER | '-' OCT_INTEGER ;
real_literal:   FLOAT |'-' FLOAT| SCIENTIFIC | '-' SCIENTIFIC ;
string_literal:  STRING_LITERAL;
boolean_literal: TRUE_ | FALSE_ ;

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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    fclose(yyin);
    yylex_destroy();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
