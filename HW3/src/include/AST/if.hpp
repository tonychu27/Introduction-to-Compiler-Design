#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"

class IfNode : public AstNode {
  public:
    IfNode(const uint32_t line, const uint32_t col, AstNode *p_expr, AstNode *p_compare1, AstNode *p_compare2);
    ~IfNode() = default;
    
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    AstNode *expr;
    AstNode *comp1;
    AstNode *comp2;
};

#endif
