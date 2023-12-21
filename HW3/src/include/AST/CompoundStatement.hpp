#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"

class CompoundStatementNode : public AstNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col, vector<AstNode*> *p_declList, vector<AstNode*> *p_stateList);
    ~CompoundStatementNode() = default;
    
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    int getDeclListLen();
    int getStateListLen();

  private:
    vector<AstNode*> *declList;
    vector<AstNode*> *stateList;
};

#endif
