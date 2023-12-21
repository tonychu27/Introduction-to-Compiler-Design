#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "decl.hpp"
#include "variable.hpp"

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col, const char *p_name, const char *p_renType, vector<AstNode*> *p_declList,  AstNode *p_compound);
    ~FunctionNode() = default;
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char *getNameCStirng() const;
    const char *getReturnTypeCString() const;
    std::string getPrototypeString() ;

  private:
    const string name;
    const string renType;
    
    vector<AstNode*> *declList;
    AstNode* compound;
};

#endif
