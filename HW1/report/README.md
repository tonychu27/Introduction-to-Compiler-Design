# hw1 report

|Field|Value|
|-:|:-|
|Name|Tony Chu|
|ID|110xxxxxx|

## How much time did you spend on this project

About 5 hours.

## Project overview

This a project for us to practice lexical analyzer for the P language.

### Implementation

#### Comment
```pascal
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
```

#### Delimiters
```pascal
"," { LIST_TOKEN(","); }
";" { LIST_TOKEN(";"); }
":" { LIST_TOKEN(":"); }
"(" { LIST_TOKEN("("); }
")" { LIST_TOKEN(")"); }
"[" { LIST_TOKEN("["); }
"]" { LIST_TOKEN("]"); }
```

#### Arithmetic, Relational, and Logical Operators
```pascal
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
```

#### Reserved Words
```pascal
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
```

#### Pseudocomments
```pascal
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
```

#### Others
```pascal
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
```

## What is the hardest you think in this project

I spent too much time dealing with C-Style comment😢.
And I didn't familiar with lex.

## Feedback to T.A.s

> Please help us improve our assignment, thanks.
