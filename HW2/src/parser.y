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

%token ',' ';' ':' '(' ')' '[' ']'
%token '+' '-' '*' '/' '%'
%token VAR DEF ARRAY OF BOOLEAN INTEGER REAL STRING TR FAL WHILE DO IF THEN ELSE FOR TO BEGIN_ END PRINT READ RETURN
%token ID DEC OCT FLOAT SCIENCE STR

%left ASS
%left AND OR NOT
%left SM LG EQ LGE NE SME
%left '-'
%left '+'
%left '/' '%'
%left '*'

%%
ProgramName
    : ID ';' VarConstDecl FuncDeclDef Compound END
    ;

    /* Function  */

FuncDecl
    : ID '(' Args ')' ';'
    | ID '(' Args ')' ':' ScalarType ';' 
    ;

FuncDef
    : ID '(' Args ')' Compound END 
    | ID '(' Args ')' ':' ScalarType Compound END 
    ;

FuncDeclDef
    : 
    | FuncDeclDef FuncDecl 
    | FuncDeclDef FuncDef
    ;

    /* Zero or more formal arguments, separated by semicolons */
Args 
    : 
    | NonEmptyArg
    ;
NonEmptyArg
    : IDList ':' ScalarType
    ;

    /* Variable */
IDList
    : ID 
    | IDList ',' ID
    ;

ScalarDecl
    : VAR IDList ':' ScalarType ';'
    | VAR IDList ':' ARRAY IntConst OF ArrType ';'
    ;

ArrType
    : ARRAY IntConst OF ArrType
    | ScalarType
    ;

IntConst
    : OCT
    | DEC
    ;

ScalarType
    : INTEGER 
    | BOOLEAN 
    | REAL 
    | STRING
    ;

LiterConstant
    : IntConst
    | FLOAT 
    | SCIENCE 
    | STR 
    | TR 
    | FAL
    ;

    /* Zero or more variable and constant declarations */
VarConstDecl
    : 
    | VarConstDecl ConstDecl 
    | VarConstDecl ScalarDecl 
    ;

ConstDecl
    : VAR IDList ':' LiterConstant ';'
    ;

    /* Statements */
Statements
    : 
    | Statements NonEmptyStatement
    ;

NonEmptyStatement
    : Compound 
    | Return 
    | Simple 
    | Condition 
    | While 
    | For 
    | FuncInvok
    ;

Compound
    : BEGIN_ VarConstDecl Statements END
    ;

Simple
    : Assign 
    | Print 
    | Read
    ;

Assign
    : VarRef ASS Expr ';'
    ;

Print
    : PRINT Expr ';'
    ;

Read
    : READ VarRef ';'
    ;

VarRef
    : ID 
    | ArrayRef
    ;

ArrayRef
    : ArrayRef '[' Expr ']' 
    | ID '[' Expr ']'
    ;

Condition
    : IF Expr THEN Compound END IF 
    | IF Expr THEN Compound ELSE Compound END IF
    ;

While
    : WHILE Expr DO Compound END DO
    ;

For 
    : FOR ID ASS IntConst TO IntConst DO Compound END DO
    ;

Return
    : RETURN Expr ';'
    ;

FuncCall
    : ID '(' ExprList ')' 
    ;
FuncInvok
    : FuncCall ';'
    ;

    /* Expression */

Expr
    : '(' Expr ')'
    | '-' Expr %prec '*'
    | Expr '*' Expr
    | Expr '/' Expr
    | Expr '%' Expr
    | Expr '+' Expr
    | Expr '-' Expr
    | Expr SM Expr
    | Expr SME Expr
    | Expr EQ Expr
    | Expr LGE Expr
    | Expr LG Expr
    | Expr NE Expr
    | Expr AND Expr
    | Expr OR Expr
    | NOT Expr
    | LiterConstant
    | VarRef 
    | FuncCall
    ;

ExprList  
    : 
    | NonEmptyExprList;
NonEmptyExprList
    : Expr 
    | NonEmptyExprList ',' Expr
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
