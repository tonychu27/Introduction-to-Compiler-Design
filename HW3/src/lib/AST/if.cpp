#include "AST/if.hpp"

IfNode::IfNode(const uint32_t line, const uint32_t col, AstNode *p_expr, AstNode *p_compare1, AstNode *p_compare2)
    : AstNode{line, col}, expr(p_expr), comp1(p_compare1), comp2(p_compare2) {}

void IfNode::print() { printf("If <line: %u, col: %u>\n", location.line, location.col); }

void IfNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(expr) { expr->accept(p_visitor); }
    if(comp1) { comp1->accept(p_visitor); }
    if(comp2) { comp2->accept(p_visitor); }
}
// Done