#include "AST/FunctionInvocation.hpp"

FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,const uint32_t col, const char *p_name, std::vector<AstNode*> *p_exprList)
    : ExpressionNode{line, col}, name(p_name), exprList(p_exprList) {}

void FunctionInvocationNode::print() { printf("Function Invocation <line: %u, col: %u>\n", location.line, location.col); }

void FunctionInvocationNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) { if(exprList){ for(auto &expr : *exprList) { expr->accept(p_visitor); } } }

const char *FunctionInvocationNode::getNameCString() const { return name.c_str(); }

// Done