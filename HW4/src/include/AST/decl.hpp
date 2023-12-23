#ifndef AST_DECL_NODE_H
#define AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/utils.hpp"
#include "AST/variable.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <memory>
#include <vector>
using namespace std;

class DeclNode final : public AstNode {
  public:
    using VarNodes = vector<unique_ptr<VariableNode>>;

  private:
    VarNodes m_var_nodes;

  private:
    void init(const vector<IdInfo> *const p_ids,
              const PTypeSharedPtr &p_type,
              ConstantValueNode *const p_constant);

  public:
    ~DeclNode() = default;

    DeclNode(const uint32_t line, const uint32_t col,
             const vector<IdInfo> *const p_ids, PType *p_type)
        : AstNode{line, col} {
        init(p_ids, PTypeSharedPtr{p_type}, nullptr);
    }

    DeclNode(const uint32_t line, const uint32_t col,
             const vector<IdInfo> *const p_ids,
             ConstantValueNode *const p_constant)
        : AstNode{line, col} {
        init(p_ids, p_constant->getTypeSharedPtr(), p_constant);
    }

    const VarNodes &getVariables() { return m_var_nodes; }

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};

#endif
