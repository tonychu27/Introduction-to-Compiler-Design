#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"

class DeclNode : public AstNode {
  public:
    DeclNode(const uint32_t line, const uint32_t col, std::vector<AstNode*> *p_varList);
    ~DeclNode() = default;
    
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    vector<AstNode*> *getVarList();

  private:
    vector<AstNode*> *varList;
};

#endif
