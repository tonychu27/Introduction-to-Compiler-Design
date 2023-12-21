%option never-interactive
%option nounput
%option noinput

%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

#define LIST_SOURCE                 appendToCurrentLine(yytext)
#define LIST_TOKEN(name)            do { LIST_SOURCE; if(opt_tok) printf("<%s>\n", name); } while(0)
#define LIST_LITERAL(name, literal) do { LIST_SOURCE; if(opt_tok) printf("<%s: %s>\n", name, literal); } while(0)
#define MAX_LINE_LENG               512

uint32_t line_num = 1;
char current_line[MAX_LINE_LENG];

static uint32_t opt_src = 1;
static uint32_t opt_tok = 1;
static char *current_line_ptr = current_line;

static void appendToCurrentLine(const char *yytext_ptr);

%}

/* Definitions */

letter [a-zA-Z]
digit [0-9]

identifier {letter}({letter}|{digit})*

decimal 0|([1-9]{digit}*)
octal 0[0-7]*
floating {decimal}\.(0|{digit}*[1-9])

float (([1-9]{digit}*)\.(0|{digit}*[1-9]))|({decimal}\.({digit}*[1-9]))
scientificNotation (([1-9]{digit}*)|{float})[eE][+-]?{decimal}


stringConstant \"((\"{2})|[^\"\n])*\"

pseudoComments "//&"[A-Z][+-].*


%x CCOMMENT

%%
    /* Transition Rules */

    /* Delimiters */
"," { LIST_TOKEN(","); return ','; }
";" { LIST_TOKEN(";"); return ';'; }
":" { LIST_TOKEN(":"); return ':'; }
"(" { LIST_TOKEN("("); return '('; }
")" { LIST_TOKEN(")"); return ')'; }
"[" { LIST_TOKEN("["); return '['; }
"]" { LIST_TOKEN("]"); return ']'; }

    /* Arithmetic, Relational, and Logical Operators */
"+" { LIST_TOKEN("+"); return '+'; }
"-" { LIST_TOKEN("-"); return '-'; }
"*" { LIST_TOKEN("*"); return '*'; }
"/" { LIST_TOKEN("/"); return '/'; }
"mod" { LIST_TOKEN("mod");return '%'; }
":=" { LIST_TOKEN(":="); return ASS; }
"<" { LIST_TOKEN("<"); return SM ; }
"<=" { LIST_TOKEN("<="); return SME; }
"<>" { LIST_TOKEN("<>"); return NE; }
">=" { LIST_TOKEN(">="); return LGE; }
">" { LIST_TOKEN(">"); return LG; }
"=" { LIST_TOKEN("="); return EQ; }
"and" { LIST_TOKEN("and"); return AND; }
"or" { LIST_TOKEN("or"); return OR; }
"not" { LIST_TOKEN("not"); return NOT; }

    /* Reserved Words */
"var" { LIST_TOKEN("KWvar"); return VAR; }
"def" { LIST_TOKEN("KWdef"); return DEF; }
"array" { LIST_TOKEN("KWarray"); return ARRAY; }
"of" { LIST_TOKEN("KWof"); return OF; }
"boolean" { LIST_TOKEN("KWboolean"); return BOOLEAN; }
"integer" { LIST_TOKEN("KWinteger"); return INTEGER; }
"real" { LIST_TOKEN("KWreal"); return REAL; }
"string" { LIST_TOKEN("KWstring"); return STRING; }
"true" { LIST_TOKEN("KWtrue"); return TR; }
"false" { LIST_TOKEN("KWfalse"); return FAL; }
"while" { LIST_TOKEN("KWwhile"); return WHILE; }
"do" { LIST_TOKEN("KWdo"); return DO; }
"if" { LIST_TOKEN("KWif"); return IF; }
"then" { LIST_TOKEN("KWthen"); return THEN; }
"else" { LIST_TOKEN("KWelse"); return ELSE; }
"for" { LIST_TOKEN("KWfor"); return FOR; }
"to" { LIST_TOKEN("KWto"); return TO; }
"begin" { LIST_TOKEN("KWbegin"); return BEGIN_;}
"end" { LIST_TOKEN("KWend"); return END; }
"print" { LIST_TOKEN("KWprint"); return PRINT; }
"read" { LIST_TOKEN("KWread"); return READ; }
"return" { LIST_TOKEN("KWreturn"); return RETURN; }

{identifier} { LIST_LITERAL("id", yytext); return ID; }
{decimal} { LIST_LITERAL("integer", yytext); return DEC; }
{octal} { LIST_LITERAL("oct_integer", yytext); return OCT; }
{floating} { LIST_LITERAL("float", yytext); return FLOAT; }
{scientificNotation} { LIST_LITERAL("scientific", yytext); return SCIENCE; }
{stringConstant} { 
    char buffer[MAX_LINE_LENG];
    int pos=0;
    for(int i=1; i<yyleng-1; i++) {
        buffer[pos++]=yytext[i];
        if (yytext[i]=='\"' && yytext[i+1]=='\"')
            i++;
    }
    buffer[pos]='\0';

    LIST_LITERAL("string", buffer);
    return STR;
}

    /* Whitespace */
[ \t]+ { LIST_SOURCE; }

{pseudoComments} {
    if (yytext[3]=='S') {
        if (yytext[4]=='+')
            opt_src=1;
        else if (yytext[4]=='-')
            opt_src=0;
    }
    else if (yytext[3]=='T') {
        if (yytext[4]=='+')
            opt_tok=1;
        else if(yytext[4]=='-')
            opt_tok=0;
    }
    
    LIST_SOURCE;
}

"//".* { LIST_SOURCE; }

    /* C Style Comment */
"/*"           { LIST_SOURCE; BEGIN(CCOMMENT); }
<CCOMMENT>"*/" { LIST_SOURCE; BEGIN(INITIAL); }
<CCOMMENT>.    { LIST_SOURCE; }

    /* Newline */
<INITIAL,CCOMMENT>\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, current_line);
    }
    ++line_num;
    current_line[0] = '\0';
    current_line_ptr = current_line;
}

    /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

/* User Subroutines */

static void appendToCurrentLine(const char *yytext_ptr) {
    while (*yytext_ptr) {
        *current_line_ptr = *yytext_ptr;
        ++current_line_ptr;
        ++yytext_ptr;
    }
    *current_line_ptr = '\0';
}