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

#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <errno.h>

#define YYLTYPE yyltype

using namespace std;

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern int32_t line_num;  /* declared in scanner.l */
extern char current_line[];     /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

static AstNode *root;
static AstNodeVisitor *visitor = new AstDumper();

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%code requires {
    class AstNode;
    class Unary;
    class Binary;
    class ConstValue;
    class Name;
    #include <vector>
    using namespace std;
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;
    char *constStr;
    float constReal;
    int constInt;

    AstNode *node;

    vector<AstNode*> *nodeList;
    vector<Name*> *nameList;
    
    struct ConstValue *ConstValue;
};

%type <identifier> ProgramName ID FunctionName
%type <nodeList> DeclarationList FunctionList FormalArgList ArrRefList ExpressionList StatementList
%type <nodeList> Declarations Functions FormalArgs ArrRefs Expressions Statements
%type <nameList> IdList
%type <node> FunctionDeclaration FunctionDefinition Statement CompoundStatement
%type <node> Function FormalArg Declaration Simple Condition While For Return 
%type <node> VariableReference ElseOrNot FunctionCall FunctionInvocation Expression
%type <constStr> ReturnType Type ScalarType ArrType ArrDecl
%type <constStr> VAR STRING_LITERAL TRUE FALSE  INTEGER REAL STRING BOOLEAN ARRAY
%type <ConstValue> LiteralConstant StringAndBoolean IntegerAndReal
%type <constInt> NegOrNot INT_LITERAL
%type <constReal> REAL_LITERAL 

    /* Follow the order in scanner.l */

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
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

%%

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {

        // printf("Biggest compound statement with decl len %d and state len %d\n", dynamic_cast<CompoundStatementNode*>$5->getDeclListLen(), dynamic_cast<CompoundStatementNode*>$5->getStateListLen());
        // if($5 == NULL)
            // printf("Compound node pointer is NULL!\n");
        root = new ProgramNode(@1.first_line, @1.first_column, 
                                $1, "void", $3, $4, $5);

        free($1);
    }
;

ProgramName:
    ID
;

DeclarationList:
    Epsilon { $$ = NULL; }
    |
    Declarations { $$ = $1; }
;

Declarations:
    Declaration {
        vector<AstNode*> *decls = new vector<AstNode*>;
        if($1 != NULL) { decls->push_back($1); }
        $$ = decls;
    }
    |
    Declarations Declaration {
        $$ = $1;
        if($2 != NULL) { $$->push_back($2); }
    }
;

FunctionList:
    Epsilon { $$ = NULL; }
    |
    Functions { $$ = $1; }
;

Functions:
    Function {
        vector<AstNode*> *f_list = new vector<AstNode*>;
        f_list->push_back($1);
        $$ = f_list;
    }
    |
    Functions Function {
        $$ = $1;
        $$->push_back($2);
    }
;

Function:
    FunctionDeclaration { $$ = $1; }
    |
    FunctionDefinition { $$ = $1; }
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON {
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $5, $3, NULL);
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END { $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $5, $3, $6); }
;

FunctionName:
    ID
;

FormalArgList:
    Epsilon { $$ = NULL; }
    |
    FormalArgs { $$ = $1; }
;

FormalArgs:
    FormalArg {
        vector<AstNode*> *decls = new vector<AstNode*>;
        decls->push_back($1);
        $$ = decls;
    }   
    |
    FormalArgs SEMICOLON FormalArg {
        $$ = $1;
        $$->push_back($3);
    }
;

FormalArg:
    IdList COLON Type {
        vector<AstNode*> *lis = new vector<AstNode*>;
        for(size_t i = 0 ; i < $1->size() ; i++){
            VariableNode *v = new VariableNode($1->at(i)->line, $1->at(i)->col, $1->at(i)->id, $3, NULL);
            lis->push_back(v);
        }
        $$ = new DeclNode(@1.first_line, @1.first_column, lis);
    }
;

IdList:
    ID {
        $$=new vector<Name*>;
        Name *n=new Name;
        n->id=$1;
        n->line=@1.first_line;
        n->col=@1.first_column;
        $$->push_back(n);
    }
    |
    IdList COMMA ID {
        $$ = $1;
        Name *n = new Name;
        n->id = $3;
        n->line = @3.first_line;
        n->col = @3.first_column;
        $$->push_back(n);
    }
;

ReturnType:
    COLON ScalarType { $$ = $2; }
    |
    Epsilon { $$ = "void"; }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON {
        vector<AstNode*> *lis = new vector<AstNode*>;
        for(size_t i = 0 ; i < $2->size(); i++){
            VariableNode *v = new VariableNode($2->at(i)->line, $2->at(i)->col, $2->at(i)->id, $4, NULL);
            lis->push_back(v);
        }
        $$ = new DeclNode(@1.first_line, @1.first_column, lis);
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON {
        vector<AstNode*> *lis = new vector<AstNode*>;
        
        char s[8];

        for(size_t i = 0 ; i < $2->size(); i++){
            ConstantValueNode *c = new ConstantValueNode($4->line, $4->col, *($4));

            if($4->intType) { strcpy(s, "integer"); }
            else if($4->realType) { strcpy(s, "real"); }
            else if($4->boolType) { strcpy(s, "boolean"); }
            else if($4->strType) { strcpy(s, "string"); }

            VariableNode *v = new VariableNode($2->at(i)->line, $2->at(i)->col, $2->at(i)->id, s, c);
            lis->push_back(v);
        }
        
        $$ = new DeclNode(@1.first_line, @1.first_column, lis);
    }
;

Type:
    ScalarType { $$ = $1; }
    |
    ArrType { $$ = $1; }
;

ScalarType:
    INTEGER { $$ = "integer"; }
    |
    REAL { $$ = "real"; }
    |
    STRING { $$ = "string"; }
    |
    BOOLEAN { $$ = "boolean"; }
;

ArrType:
    ArrDecl ScalarType {
        char s[3][32];

        strcpy(s[0], $2);
        strcpy(s[2], $1);
        strcpy(s[1], " ");

        strcat(s[1], s[2]);
        strcat(s[0], s[1]);

        $$ = s[0];
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF {
        char s[2][32];

        strcpy(s[0], "[");
        int val = $2;

        sprintf(s[1], "%d", val);
        strcat(s[0], s[1]);
        strcpy(s[1], "]");
        strcat(s[0], s[1]);
        $$ = s[0];
    }
    |
    ArrDecl ARRAY INT_LITERAL OF {
        char s1[32];
        char s2[32];
        
        strcpy(s1, $1);
        strcpy(s2, "[");
        strcat(s1, s2);

        int val = $3;
        sprintf(s2, "%d", val);
        
        strcat(s1, s2);
        strcpy(s2, "]");
        strcat(s1, s2);

        $$ = s1;
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL {
        $$ = new ConstValue;
        $$->intValue = $1 * $2;
        $$->intType = 1;
        
        if($1 == 1) { $$->col = @2.first_column, $$->line = @2.first_line; }
        else { $$->col = @1.first_column, $$->line = @1.first_line; }
    }
    |
    NegOrNot REAL_LITERAL {
        $$ = new ConstValue;
        $$->realValue = $1 * $2;
        $$->realType = 1;
        if($1 == 1) { $$->col = @2.first_column, $$->line = @2.first_line; }
        else { $$->col = @1.first_column, $$->line = @1.first_line; }
    }
    |
    StringAndBoolean {
        $$ = $1;
        $$->col = @1.first_column;
        $$->line = @1.first_line;
    }
;

NegOrNot:
    Epsilon { $$ = 1; }
    |
    MINUS %prec UNARY_MINUS { $$ = -1; }
;

StringAndBoolean:
    STRING_LITERAL {
        $$ = new ConstValue;
        $$->strValue = $1;
        $$->strType = 1;
    }
    |
    TRUE {
        $$ = new ConstValue;
        $$->strValue = "true";
        $$->boolType = 1;
    }
    |
    FALSE {
        $$ = new ConstValue;
        $$->strValue = "false";
        $$->boolType = 1;
    }
;

IntegerAndReal:
    INT_LITERAL {
        $$ = new ConstValue;
        $$->intValue = $1;
        $$->intType = 1;
    }
    |
    REAL_LITERAL {
        $$ = new ConstValue;
        $$->realValue = $1;
        $$->realType = 1;
    }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement { $$ = $1; }
    |
    Simple { $$ = $1; }
    |
    Condition { $$ = $1; }
    |
    While { $$ = $1; }
    |
    For { $$ = $1; }
    |
    Return { $$ = $1; }
    |
    FunctionCall { $$ = $1; }
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END { $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3); }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON { $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, $3); }
    |
    PRINT Expression SEMICOLON { $$ = new PrintNode(@1.first_line, @1.first_column, $2); }
    |
    READ VariableReference SEMICOLON { $$ = new ReadNode(@1.first_line, @1.first_column, $2); }
;

VariableReference:
    ID ArrRefList { $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2); }
;

ArrRefList:
    Epsilon {$$ = NULL; }
    |
    ArrRefs { $$ = $1; }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET {
        vector<AstNode*> *v = new vector<AstNode*>;
        v->push_back($2);
        $$ = v;
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET {
        $$ = $1;
        $$->push_back($3);
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF {
        if($4 == NULL) { $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, NULL); }
        else { $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5); }
    }
;

ElseOrNot:
    ELSE
    CompoundStatement { $$ = $2; }
    |
    Epsilon { $$ = NULL; }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO { $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4); }
;

    /* Do it later, too difficult */
For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO {
        vector<AstNode*> *lis = new vector<AstNode*>;
        VariableNode *v = new VariableNode(@2.first_line, @2.first_column, $2, "integer", NULL);
        lis->push_back(v);
        DeclNode *decl = new DeclNode(@2.first_line, @2.first_column, lis);

        VariableReferenceNode *var = new VariableReferenceNode(@2.first_line, @2.first_column, $2, NULL);
        ConstValue *c = new ConstValue;
        c->intValue = $4;
        c->intType = 1;
        ConstantValueNode *c1 = new ConstantValueNode(@4.first_line, @4.first_column, *(c));
        AssignmentNode *a = new AssignmentNode(@3.first_line, @3.first_column, var, c1);

        ConstValue *cc = new ConstValue;
        cc->intValue = $6;
        cc->intType = 1;
        ConstantValueNode *c2 = new ConstantValueNode(@6.first_line, @6.first_column, *(cc));

        $$ = new ForNode(@1.first_line, @1.first_column, decl, a, c2, $8);
    }
;

Return:
    RETURN Expression SEMICOLON { $$ = new ReturnNode(@1.first_line, @1.first_column, $2); }
;

FunctionCall:
    FunctionInvocation SEMICOLON { $$ = $1; }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS { $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3); }
;

ExpressionList:
    Epsilon { $$ = NULL; }
    |
    Expressions { $$ = $1; }
;

Expressions:
    Expression
    {
        vector<AstNode*> *v = new vector<AstNode*>;
        v->push_back($1);
        $$ = v;
    }
    |
    Expressions COMMA Expression
    {
        $$ = $1;
        $$->push_back($3);
    }
;

StatementList:
    Epsilon { $$ = NULL; }
    |
    Statements { $$ = $1; }
;

Statements:
    Statement
    {
        vector<AstNode*> *v = new vector<AstNode*>;
        if($1 != NULL) { v->push_back($1); }

        $$ = v;
    }
    |
    Statements Statement
    {
        $$ = $1;

        if($2 != NULL) { $$->push_back($2); }
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS { $$ = $2; }
    |
    MINUS Expression %prec UNARY_MINUS
    {
        Unary u;
        u.neg = 1;

        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, u ,$2);
    }
    |
    Expression MULTIPLY Expression
    {
        Binary b;
        b.mul = 1;

        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    Expression DIVIDE Expression
    {
        Binary b;
        b.div = 1;

        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    Expression MOD Expression
    {
        Binary b;
        b.mod = 1;

        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    Expression PLUS Expression
    {
        Binary b;
        b.add = 1;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    Expression MINUS Expression
    {
        Binary b;
        b.sub = 1;

        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    Expression LESS Expression
    {
        Binary b;
        b.sm = 1;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    Expression LESS_OR_EQUAL Expression
    {
        Binary b;
        b.sme = 1;

        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    Expression GREATER Expression
    {
        Binary b;
        b.lg = 1;

        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    Expression GREATER_OR_EQUAL Expression
    {
        Binary b;
        b.lge = 1;

        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    Expression EQUAL Expression
    {
        Binary b;
        b.eq = 1;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    Expression NOT_EQUAL Expression
    {
        Binary b;
        b.ne = 1;

        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    NOT Expression
    {
        Unary u;
        u.NOT = 1;

        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, u, $2);
    }
    |
    Expression AND Expression
    {
        Binary b;
        b.AND = 1;

        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    Expression OR Expression
    {
        Binary b;
        b.OR = 1;

        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, b, $1, $3);
    }
    |
    IntegerAndReal { $$ = new ConstantValueNode(@1.first_line, @1.first_column, *($1)); }
    |
    StringAndBoolean { $$ = new ConstantValueNode(@1.first_line, @1.first_column, *($1)); }
    |
    VariableReference { $$ = $1; }
    |
    FunctionInvocation { $$ = $1; }
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
    if (argc < 2) {
        fprintf(stderr, "Usage: ./parser <filename> [--dump-ast]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed:");
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        root->accept(*visitor);
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
