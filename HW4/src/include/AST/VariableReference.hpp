#ifndef AST_VARIABLE_REFERENCE_NODE_H
#define AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <memory>
#include <string>
#include <vector>
using namespace std;

class VariableReferenceNode final : public ExpressionNode {
  public:
    using ExprNodes = vector<unique_ptr<ExpressionNode>>;

  private:
    string m_name;
    ExprNodes m_indices;

  public:
    ~VariableReferenceNode() = default;

    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          const char *const p_name)
        : ExpressionNode{line, col}, m_name(p_name){}

    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          const char *const p_name, ExprNodes &p_indices)
        : ExpressionNode{line, col}, m_name(p_name),
          m_indices(move(p_indices)){}

    const char *getNameCString() const { return m_name.c_str(); }
    int checkInvalidChildren();
    int getIndicesNum() const;
    int checkNonIntegerIndices();

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};

#endif
