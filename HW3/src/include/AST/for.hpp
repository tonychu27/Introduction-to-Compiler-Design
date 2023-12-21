#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col, AstNode *p_decl, AstNode *p_assign, AstNode *p_expr, AstNode *p_compound);
    ~ForNode() = default;
    
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    AstNode* decl;
    AstNode* assign;
    AstNode* expr;
    AstNode* compound;
};

#endif
