#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"

#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col, Binary p_bin, AstNode *p_left, AstNode *p_right);
    ~BinaryOperatorNode() = default;
    
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char *getOperatorCString() const;

  private:
    // TODO: operator, expressions
    Binary op;
    AstNode *left;
    AstNode *right;
};

#endif
