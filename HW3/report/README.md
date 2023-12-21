# hw3 report

|||
|-:|:-|
|Name|Tony Chu|
|ID|110xxxxxx|

## How much time did you spend on this project

About 4 hours for reading the spec, 26 hours for coding, and 1 hour for writing report. \
Total: About 31 hours. 😵‍💫

## Project overview

This is a project for us to practice AST(abstract syntax tree) generated for `P` language.
### Implementation overview 
#### In **scanner.l**
Add ```yylval.identifier = strndup(yytext, MAX_ID_LENG);``` to `identifier`. \
Add `yylval.constInt = atoi(yytext);` to `integer` and `octal number`. \
Add `yylval.constReal = atof(yytext);` to `float`. \
Add `char* buf = strdup(string_literal); yylval.constStr = buf;` to `string`.
#### In **parser.y**
**%union** \
I define these types. 
```c++ 
char *identifier;
char *constStr; 
float constReal;
int constInt;

/***  A pointer to AstNode ***/
class AstNode *node; 

/*** A vector pointer for any grammar rule who ends with List(Except IDList, I used nameList instead) or s ***/
vector<AstNode*> *nodeList;

/*** A vector pointer for IDList ***/
vector<Name*> *nameList;

/*** For LiteralConstant, StringAndBoolean and IntegerAndReal ***/
struct ConstValue *ConstValue;
```
**Syntax Rules** \
Create each node dynamically by ```new``` .

**Visitor Pattern** \
Each derived class from ```AstNode``` must implement ```void accept(AstNodeVisitor &p_visitor)``` and ```void visitChildNodes(AstNodeVisitor &p_visitor)```. \
Pass visitor by reference ```accept(p_visitor);``` and it will call ```p_visitor.visit(*this);```. \
And the visit will be done by ```AstDumper``` sequentially.
#### For Exampls
In ```FunctionInvocation.cpp```, it will invoke ```accept``` of each child node sequentially.
```c++
/*** Accept each node in exprList sequentially ***/
void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) { if(exprList){ for(auto &expr : *exprList) { expr->accept(p_visitor); } } }
```

## What is the hardest you think in this project

1. Spending too many time reading spec and understanding it. 
2. I can't use ```C++ string``` in ```parser.y```, I used ```char*``` instead. 
3. How to present ```type``` and ```operator``` in ```P Language```. 
4. Easy to have segmentation fault.
5. Too many things to do in this assignment.

## Feedback to T.A.s
Spec is very detailed 👍👍👍. \
This homework is harder than previous two, however, it help me remind the concept of OOP in C++. \
I think the introduction of [tutorial of visitor pattern](../tutorial/) is very funny, espically ```編義氣```, and indeed, I don't
need ```LLC``` to help me get my life companion. 🤣🤣🤣 (Just want to thank you for providing something funny in this assignment, making it not that boring! 😃) \
Last but not least, I want to thank TAs for giving us template, helping me reduce lots of time of doing homework. (But I still spent more than 24 hours 😢, too many files to be implemented. 😖) 
