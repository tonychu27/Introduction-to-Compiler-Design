#ifndef SEMA_SEMANTIC_ANALYZER_H
#define SEMA_SEMANTIC_ANALYZER_H

#include "visitor/AstNodeVisitor.hpp"
#include <vector>
#include <stack>
#include <string>
using namespace std;

#define OUTPUTFORMAT          "%-33s%-11s%-11s%-17s%-11s\n"
#define OUTPUTENTRY           "%-33s%-11s%d%-10s%-17s%-11s\n"
#define MAX_LINE_NUM        512
extern uint32_t opt_dump;
extern char *code[MAX_LINE_NUM];
extern bool error;

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

class SemanticAnalyzer final : public AstNodeVisitor {
  private:
    bool flagFor = 0;
    bool flagLoopVar = 0;
    bool flagFunction = 0;
    bool flagInFunction = 0;
    bool flagVar = 0;
    int mLevel = 0;
    
    string currentFunctionName;
    SymbolManager *symbolManager;
    SymbolManager *tempManager;
    SymbolTable *currentTable;
    SymbolEntry *currentEntry;

  public:
    ~SemanticAnalyzer() = default;
    SemanticAnalyzer() = default;

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;
};

#endif
