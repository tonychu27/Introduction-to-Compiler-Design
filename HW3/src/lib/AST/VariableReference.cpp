#include "AST/VariableReference.hpp"

VariableReferenceNode::VariableReferenceNode(const uint32_t line,const uint32_t col, const char *p_name, std::vector<AstNode*> *p_exprList)
    : ExpressionNode{line, col}, name(p_name), exprList(p_exprList) {}

void VariableReferenceNode::print() { printf("Variable Reference <line: %u, col: %u>\n", location.line, location.col); }

void VariableReferenceNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) { if(exprList != NULL){ for(auto &expr:*exprList) { expr->accept(p_visitor); } } }

const char *VariableReferenceNode::getNameCString() const { return name.c_str(); }

// Done