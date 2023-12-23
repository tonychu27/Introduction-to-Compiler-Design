#ifndef AST_IF_NODE_H
#define AST_IF_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"

#include <memory>
using namespace std;

class IfNode final : public AstNode {
  private:
    unique_ptr<ExpressionNode> m_condition;
    unique_ptr<CompoundStatementNode> m_body;
    unique_ptr<CompoundStatementNode> m_else_body;

  public:
    ~IfNode() = default;
    IfNode(const uint32_t line, const uint32_t col,
           ExpressionNode *p_condition, CompoundStatementNode *p_body,
           CompoundStatementNode *p_else_body)
        : AstNode{line, col}, m_condition(p_condition), m_body(p_body),
          m_else_body(p_else_body){}
    const int checkInvalidCondition() const;
    const int checkConditionBoolType() const;
    const uint32_t getConditionLocationCol() const;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};

#endif
