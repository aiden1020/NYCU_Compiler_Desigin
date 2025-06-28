%option never-interactive
%option nounput
%option noinput

%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 512
/* Code runs each time a token is matched. */
#define YY_USER_ACTION updateCurrentLine(yytext);
  
char *buffer = NULL;  
size_t buf_size = 10;

uint32_t line_num = 1;
uint32_t col_num = 1;
static uint32_t opt_src = 1;
static uint32_t opt_tok = 1;
static char current_line[MAX_LINE_LEN];

static void updateCurrentLine(const char *source);
static void listToken(const char *name);
static void listLiteral(const char *name, const char *literal);
static char* concatToken(const char *name);
static void handlingPseudocomment(const char *pseudocomment);
static void handlingScientificZero(const char *name);
%}

%x COMMENT
%x STRING_CONSTANT

PW_DECLARATION "var"|"def"
PW_TYPE "array"|"of"|"boolean"|"integer"|"real"|"string"
PW_VALUE "true"|"false"
PW_FLOW_CONTROL "while"|"do"|"if"|"then"|"else"|"for"|"to"
PW_BLOCK "begin"|"end"
PW_STATEMENT "print"|"read"|"return"

DELIMITER [,\;\:\(\)\[\]]
OP_ARITHMETIC [+\-\*\/]|"mod"|":=" 
OP_RELATIONAL "<="|"<"|"<>"|">="|">"|"="
OP_LOGICAL "and"|"or"|"not"

ID [A-Za-z][A-Za-z0-9]*
INT_DECIMAL ([1-9][0-9]*)|0
INT_OCTAL 0[0-7]+
FLOAT (0|{INT_DECIMAL})[.]([0]*[1-9]+|0) 
SCIENTIFIC_ZERO "0.0"[eE][-+]?{INT_DECIMAL}
SCIENTIFIC ({FLOAT}|([1-9][0-9]*))[eE][-+]?{INT_DECIMAL}
SPACE [ \t\n\r]+
COMMENT_SINGLE \/\/.*
PSEUDOCOMMENT \/\/&[A-Z][+-].*

%%
<INITIAL>\/\* {BEGIN(COMMENT);}
<COMMENT>[^*]*        
<COMMENT>\*
<COMMENT>\*\/ {BEGIN(INITIAL);}

<INITIAL>\" { buffer = (char*)malloc(buf_size);buffer[0] = '\0';BEGIN(STRING_CONSTANT);}  
<STRING_CONSTANT>[^"\n]+ { 
    size_t org_len = strlen(buffer);
    size_t new_len = org_len + strlen(yytext); 
    
    if (new_len > buf_size) {   
        while (new_len > buf_size) {
            buf_size += 64;  
        }
        buffer = (char*)realloc(buffer, buf_size);
    }
    strcat(buffer, yytext);
}  
<STRING_CONSTANT>\"\" { 
    size_t org_len = strlen(buffer);
    size_t new_len = org_len + 2;  

    if (new_len > buf_size) {  
        buf_size += 64;
        buffer = (char*)realloc(buffer, buf_size);
    }
    strcat(buffer, "\"");
}  
<STRING_CONSTANT>\" { BEGIN(INITIAL); listLiteral("string", buffer);free(buffer);buffer = NULL;buf_size = 10;}  
<STRING_CONSTANT>\n { BEGIN(INITIAL); free(buffer);buffer = NULL;buf_size = 10;}  

{PW_DECLARATION}|{PW_TYPE}|{PW_VALUE}|{PW_FLOW_CONTROL}|{PW_BLOCK}|{PW_STATEMENT} { listToken(concatToken(yytext)); }
{DELIMITER}|{OP_ARITHMETIC}|{OP_RELATIONAL}|{OP_LOGICAL} { listToken(yytext); }

{ID} { listLiteral("id",yytext); }
{INT_OCTAL} { listLiteral("oct_integer",yytext); }
{INT_DECIMAL} { listLiteral("integer",yytext); }
{FLOAT} { listLiteral("float",yytext); }

{SCIENTIFIC_ZERO} { handlingScientificZero(yytext); }
{SCIENTIFIC} { listLiteral("scientific",yytext); }

{PSEUDOCOMMENT} { handlingPseudocomment(yytext); }
{SPACE}|{COMMENT_SINGLE}

. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}
%%

/** @note The line is printed out and flushed when a newline character is encountered. */
static void updateCurrentLine(const char *source) {
    /* col_num is one-based */
    for (const char *c = source; *c; ++c) {
        if (*c == '\n') {
            if (opt_src) {
                printf("%d: %s\n", line_num, current_line);
            }
            ++line_num;
            col_num = 1;
            current_line[0] = '\0';
        } else {
            if (col_num < MAX_LINE_LEN) {
                current_line[col_num - 1] = *c;
                current_line[col_num] = '\0';
            } else {
                /* Truncate silently; doesn't affect the program's correctness. */
            }
            ++col_num;
        }
    }
}

static void listToken(const char *name) {
    if (opt_tok) {
        printf("<%s>\n", name);
    }
}

static void listLiteral(const char *name, const char *literal) {
    if (opt_tok) {
        printf("<%s: %s>\n", name, literal);
    }
}
static char* concatToken(const char *name) {
    static char token[256];
    token[0] = '\0';
    strcat(token, "KW");
    strcat(token, name);
    return token; 
}

static void handlingPseudocomment(const char *pseudocomment) {
    char option = pseudocomment[3]; 
    char setting = pseudocomment[4]; 

    switch (option) {
        case 'S':
            if (setting == '+') {
                opt_src = 1; 
            } else if (setting == '-') {
                opt_src = 0;  
            }
            break;
        case 'T':
            if (setting == '+') {
                opt_tok = 1;  
            } else if (setting == '-') {
                opt_tok = 0;                   
            }
        default:
            break;
    }
}
static void handlingScientificZero(const char *name) {
    listLiteral("float", "0.0");
    listLiteral("id", &name[3]); 
}


/** @note This function is not required if the input file is guaranteed to end
 * with a newline. However, students may find it useful to handle the case where
 * the input file does not end with a newline, as it has been reported several
 * times in the past.
 */
int yywrap(void) {
    /* If the file is not ended with a newline, fake it to print out the last line. */
    if (col_num > 1) {
        updateCurrentLine("\n");
    }
    /* no more input file */
    return 1;
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    fclose(yyin);
    /* Free resources used by the scanner; otherwise, unused data structures
    will persist throughout the successive compiler phases. */
    yylex_destroy();

    return 0;
}
