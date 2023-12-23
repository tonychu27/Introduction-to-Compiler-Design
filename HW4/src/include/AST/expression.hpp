#ifndef AST_EXPRESSION_NODE_H
#define AST_EXPRESSION_NODE_H

#include "AST/ast.hpp"
#include "AST/PType.hpp"
using namespace std;

class ExpressionNode : public AstNode {
  public:
    ~ExpressionNode() = default;
    ExpressionNode(const uint32_t line, const uint32_t col)
        : AstNode{line, col} {}
    const char *getPTypeCString();
    void setNodeType(string type_string);
    void setNodeTypeDimensions(vector<uint64_t> dims);
  protected:
    PTypeSharedPtr type = nullptr;
};

#endif
