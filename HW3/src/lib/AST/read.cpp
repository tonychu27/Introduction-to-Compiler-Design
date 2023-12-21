#include "AST/read.hpp"

ReadNode::ReadNode(const uint32_t line, const uint32_t col, AstNode *p_varRef)
    : AstNode{line, col}, varRef(p_varRef) {}

void ReadNode::print() { printf("Read <line: %u, col: %u>\n", location.line, location.col); }

void ReadNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) { if(varRef) { varRef->accept(p_visitor); } }
// Done