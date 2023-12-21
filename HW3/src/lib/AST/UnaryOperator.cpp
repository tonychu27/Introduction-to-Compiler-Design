#include "AST/UnaryOperator.hpp"

UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col, Unary p_op, AstNode *p_operand)
    : ExpressionNode{line, col}, op(p_op), operand(p_operand) {}

void UnaryOperatorNode::print() { printf("Unary Operator <line: %u, col: %u>\n", location.line, location.col); }

void UnaryOperatorNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) { if(operand != NULL) operand->accept(p_visitor); }

const char *UnaryOperatorNode::getOperatorCString() const{
    if(op.neg) return "neg";
    if(op.NOT) return "not";
}