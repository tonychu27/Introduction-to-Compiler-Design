#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"

class VariableReferenceNode : public ExpressionNode {
  public:
    VariableReferenceNode(const uint32_t line, const uint32_t col, const char *p_name, vector<AstNode*> *p_exprList);
    ~VariableReferenceNode() = default;
    
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char* getNameCString() const;

  private:
    const string name;
    vector<AstNode*> *exprList;
};

#endif
