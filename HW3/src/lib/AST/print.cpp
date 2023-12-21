#include "AST/print.hpp"

PrintNode::PrintNode(const uint32_t line, const uint32_t col, AstNode *p_expr)
    : AstNode{line, col}, expr(p_expr) {}

void PrintNode::print() { printf("Print <line: %u, col: %u>\n", location.line, location.col); }

void PrintNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) { if(expr != NULL) { expr->accept(p_visitor); } }
// Done