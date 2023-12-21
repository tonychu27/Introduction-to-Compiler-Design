#include "AST/for.hpp"

ForNode::ForNode(const uint32_t line, const uint32_t col, AstNode *p_decl, AstNode *p_assign, AstNode *p_expr, AstNode *p_compound)
    : AstNode{line, col}, decl(p_decl), assign(p_assign), expr(p_expr), compound(p_compound) {}

void ForNode::print() { printf("For <line: %u, col: %u>\n", location.line, location.col); }

void ForNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(decl) { decl->accept(p_visitor); }
    if(assign) { assign->accept(p_visitor); }
    if(expr) { expr->accept(p_visitor); }
    if(compound) { compound->accept(p_visitor); }
}

// Done
