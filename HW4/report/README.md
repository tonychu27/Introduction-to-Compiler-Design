# hw4 report

|||
|-:|:-|
|Name|Tony Chu|
|ID|110xxxxxx|

## How much time did you spend on this project

About 2 hours for reading the spec, 30 hours for coding, and 1 hour for writing report. \
Total: About 33 hours. 😵‍💫

## Project overview

This is a project for us to practice semantic table generation for `P` language. \
In this project, I used the AST built last time and added a new visor (that is, semantic analyzer) to visit that AST, and then in the process, we built a symbol table and performed semantic analysis. 

### Implementation overview 
#### In **scanner.l**
Add `char *code[MAX_LINE_NUM];` each line of source code in an array of pointer to `char`. \
Add `uint32_t opt_dump = 1;` to handle symbol table dumping. \
Add `code[line_num] = strdup(current_line);` to record each line of source code. 

#### In **parser.y**
Add `bool error = 0;` to indicate there is an error or not. \
```c++
if(!error) {
        printf("\n"
            "|---------------------------------------------------|\n"
            "|  There is no syntactic error and semantic error!  |\n"
            "|---------------------------------------------------|\n");
    }
```

#### My Symbol Table
In `SemanticAnalyzer.hpp`, I add the following three `class` to help me. 

##### Symbol Entry 
Presents a single entry in the symbol table.

```c++
class SymbolEntry{
  public:
    SymbolEntry(const char *p_variableName, const char* p_kind, const int p_level, const char *p_type, const char *p_attribute)
    : variableName(p_variableName), kind(p_kind), level(p_level), type(p_type), attribute(p_attribute){}
    void dumpEntry(void);
    const char *getNameCString() const;
    const char *getTypeCString() const;
    const char *getKindCString() const;
    const char *getAttributeCString() const;
    int getTypeDimension() const;
    void setKindString(const char *p_kind);
    void setAttributeString(const char *p_attribute);
    void getNewTypeDimensions(vector<uint64_t> &dims, int ignore_before) const;
    int getFunctionParameterCount() const;
  private:
    string variableName;
    string kind;
    int level;
    string type;
    string attribute;
};
```
##### Symbol Table
Presents the symbol table. \
Has a list of SymbolEntrys. (I used `vector` to represent it) \
`addSymbol` to add a new symbol into this symbol table. 

```c++
class SymbolTable{
  public:
    SymbolTable(){}
    void addSymbol(SymbolEntry *entry);
    void dumpSymbol(void);
    int checkRedeclaration(const char *p_name) const;
    void addErrorSymbol(const char *p_name);
    int checkErrorDeclare(const char *p_name) const;
    SymbolEntry *getEntry(const char *p_name);
  private:
    vector<SymbolEntry *> entries;
    vector<const char *> errorDeclaration;
};
```

##### Symbol Manager
Has a list of SymbolTables.

```c++
class SymbolManager{
  public:
    SymbolManager(){}
    void pushScope(SymbolTable *new_scope);
    void popScope(void);
    SymbolTable* topScope(void);
    int checkLoopVariableRedeclaration(const char *);
    int checkConstant(const char *);
    void pushLoopVar(const char *p_name);
    void popLoopVar(void);
    void pushConstant(const char *p_name);
    void popConstant(void);
    int getScopeSize(void);
  private:
    stack <SymbolTable *> tables;
    vector <const char *> loopVar;
    vector <const char *> constants;
};
```

#### Show Error
Write error message to `stderr`.

```c++
static void showErrorMessage(uint32_t line, uint32_t column, string errorMessage){
    cerr << "<Error> Found in line "<< line << ", column " << column << ": " << errorMessage << endl;
    cerr << "    " << code[line] << endl;
    for (int i = 1; i < column + 4; i++) cerr << " ";
    cerr << "^" << endl;
}
```

## What is the hardest you think in this project
I think this assignment is much harder than the last assignment. 

I think the hardest part is creating the table and setting the type of the expression node. There were a lot of scope things to consider when building the table, which was not very clear at first, so I spent some time researching and modifying it. As for type, I added a lot of functions myself, such as changing dimensions to mutable or doing a lot of string processing to get the type of node in the symbol table, which was quite hard. 

And too many things to be implemented.

## Feedback to T.A.s
Spec is very detailed 👍👍👍. \
However, I think this assignment should be released one week earily since this assignment is **very very hard** and the deadline is **very close** to our finals week, so the pressure is very very high. Thus, I think it is fine if assignment 4 is released before the deadline of assignment 3 (Although you have provided the spec of HW4 with HW3, I believe that some people want to do it earily) \
The `CI / Grade (push)` in these assignment is very useful, the previous three assignments don't have it. I can make sure that my code pass all the test on GitHub. \
Last but not least, I want to thank TAs for giving us template, helping me reduce lots of time of doing homework.