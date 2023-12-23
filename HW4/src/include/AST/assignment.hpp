#ifndef AST_ASSIGNMENT_NODE_H
#define AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/VariableReference.hpp"

#include <memory>
using namespace std;

class AssignmentNode final : public AstNode {
  private:
    unique_ptr<VariableReferenceNode> mLvalue;
    unique_ptr<ExpressionNode> mExpr;

  public:
    ~AssignmentNode() = default;
    AssignmentNode(const uint32_t line, const uint32_t col,
                   VariableReferenceNode *p_var_ref, ExpressionNode *p_expr)
        : AstNode{line, col}, mLvalue(p_var_ref), mExpr(p_expr){}
    const char *getConstantValueCString();
    const char *getLvalueNameCString();
    const char *getLvalueTypeCString();
    const char *getRvalueTypeCString();
    const uint32_t getLvalueLocationCol() const;
    const uint32_t getRvalueLocationCol() const;
    const int checkLvalue() const;
    const int checkRvalue() const;
    const int checkLvalueScalarType() const;
    const int checkRvalueScalarType() const;
    const int checkCompatibleLRvalueType() const;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};

#endif
