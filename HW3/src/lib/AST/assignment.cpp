#include "AST/assignment.hpp"

AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col, AstNode *p_varRef, AstNode *p_expr)
    : AstNode{line, col}, varRef(p_varRef), expr(p_expr) {}

void AssignmentNode::print() { printf("assignment statement <line: %u, col: %u>\n", location.line, location.col); }

void AssignmentNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    
    if(varRef) { varRef->accept(p_visitor); }
    if(expr) { expr->accept(p_visitor); }
}

// Done