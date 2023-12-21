%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LIST_SOURCE                 appendToCurrentLine(yytext)
#define LIST_TOKEN(name)            do { LIST_SOURCE; if(opt_tok) printf("<%s>\n", name); } while(0)
#define LIST_LITERAL(name, literal) do { LIST_SOURCE; if(opt_tok) printf("<%s: %s>\n", name, literal); } while(0)
#define MAX_LINE_LENG               512

uint32_t line_num = 1;
static char current_line[MAX_LINE_LENG];

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


%x C_STYLE
%x C_PLUS_STYLE

%%
    /* Transition Rules */

    /* Discard */
" "|\t { LIST_SOURCE; }
<INITIAL>"//"	{ LIST_SOURCE; BEGIN(C_PLUS_STYLE); }
<C_PLUS_STYLE>\n {
    BEGIN(INITIAL);
    if(opt_src)
        printf("%d: %s\n", line_num, current_line);
    line_num++;
    current_line[0]='\0';
    current_line_ptr=current_line; 
}
<C_PLUS_STYLE>. { LIST_SOURCE; }

<INITIAL>"/*"	{ LIST_SOURCE; BEGIN(C_STYLE); }
<C_STYLE>\n {
    BEGIN(C_STYLE);
    if(opt_src)
        printf("%d: %s\n", line_num, current_line);
    line_num++;
    current_line[0]='\0';
    current_line_ptr=current_line; 
}
<C_STYLE>"*/"	{ LIST_SOURCE; BEGIN (INITIAL); }
<C_STYLE>.	{ LIST_SOURCE; }

    /* Delimiters */
"," { LIST_TOKEN(","); }
";" { LIST_TOKEN(";"); }
":" { LIST_TOKEN(":"); }
"(" { LIST_TOKEN("("); }
")" { LIST_TOKEN(")"); }
"[" { LIST_TOKEN("["); }
"]" { LIST_TOKEN("]"); }

    /* Arithmetic, Relational, and Logical Operators */
"+" { LIST_TOKEN("+"); }
"-" { LIST_TOKEN("-"); }
"*" { LIST_TOKEN("*"); }
"/" { LIST_TOKEN("/"); }
"mod" { LIST_TOKEN("mod"); }
":=" { LIST_TOKEN(":="); }
"<" { LIST_TOKEN("<"); }
"<=" { LIST_TOKEN("<="); }
"<>" { LIST_TOKEN("<>"); }
">=" { LIST_TOKEN(">="); }
">" { LIST_TOKEN(">"); }
"=" { LIST_TOKEN("="); }
"and" { LIST_TOKEN("and"); }
"or" { LIST_TOKEN("or"); }
"not" { LIST_TOKEN("not"); }

    /* Reserved Words */
"var" { LIST_TOKEN("KWvar"); }
"def" { LIST_TOKEN("KWdef"); }
"array" { LIST_TOKEN("KWarray"); }
"of" { LIST_TOKEN("KWof"); }
"boolean" { LIST_TOKEN("KWboolean"); }
"integer" { LIST_TOKEN("KWinteger"); }
"real" { LIST_TOKEN("KWreal"); }
"string" { LIST_TOKEN("KWstring"); }
"true" { LIST_TOKEN("KWtrue"); }
"false" { LIST_TOKEN("KWfalse"); }
"while" { LIST_TOKEN("KWwhile"); }
"do" { LIST_TOKEN("KWdo"); }
"if" { LIST_TOKEN("KWif"); }
"then" { LIST_TOKEN("KWthen"); }
"else" { LIST_TOKEN("KWelse"); }
"for" { LIST_TOKEN("KWfor"); }
"to" { LIST_TOKEN("KWto"); }
"begin" { LIST_TOKEN("KWbegin"); }
"end" { LIST_TOKEN("KWend"); }
"print" { LIST_TOKEN("KWprint"); }
"read" { LIST_TOKEN("KWread"); }
"return" { LIST_TOKEN("KWreturn"); }

{identifier} { LIST_LITERAL("id", yytext); }
{decimal} { LIST_LITERAL("integer", yytext); }
{octal} { LIST_LITERAL("oct_integer", yytext); }
{floating} { LIST_LITERAL("float", yytext); }
{scientificNotation} { LIST_LITERAL("scientific", yytext); }
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
}

\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, current_line);
    }
    ++line_num;
    current_line[0] = '\0';
    current_line_ptr = current_line;
}

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