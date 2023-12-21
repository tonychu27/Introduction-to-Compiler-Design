#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col, const char *p_name, const char *p_type, AstNode *p_const);
    ~VariableNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    string getType();
    const char *getTypeCString();
    const char *getNameCString();

  private:
    const string name;
    const string type;
    AstNode *constant;
};

#endif
