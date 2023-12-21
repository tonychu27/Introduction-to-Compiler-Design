#ifndef AST_PROGRAM_NODE_H
#define AST_PROGRAM_NODE_H

#include "AST/ast.hpp"

#include <string>

class ProgramNode final : public AstNode {
  public:
    ~ProgramNode() = default;
    ProgramNode(const uint32_t line, const uint32_t col, const char *p_name, const char *p_renType, vector<AstNode*> *p_declList, vector<AstNode*> *p_funcList, AstNode *p_compound);

    const char *getNameCString() const;
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    const string name;
    const string renType;
    vector<AstNode*> *declList;
    vector<AstNode*> *funcList;
    AstNode *compound;
};

#endif
