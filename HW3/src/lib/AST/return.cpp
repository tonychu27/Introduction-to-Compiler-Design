#include "AST/return.hpp"

ReturnNode::ReturnNode(const uint32_t line, const uint32_t col, AstNode *p_retVal)
    : AstNode{line, col}, retVal(p_retVal) {}

void ReturnNode::print() { printf("Return <line: %u, col: %u>\n", location.line, location.col); }

void ReturnNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) { if(retVal) retVal->accept(p_visitor); }

// Done