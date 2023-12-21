#include "AST/BinaryOperator.hpp"

BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col, Binary p_bin, AstNode *p_left, AstNode *p_right)
    : ExpressionNode{line, col}, op(p_bin), left(p_left), right(p_right) {}

void BinaryOperatorNode::print() { printf("Binary Operator <line: %u, col: %u>\n", location.line, location.col); }

void BinaryOperatorNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(left != NULL) { left->accept(p_visitor); }
    if(right != NULL) { right->accept(p_visitor); }
}

const char *BinaryOperatorNode::getOperatorCString() const{
    if(op.mul)
        return "*";
    else if (op.div)
        return "/";
    else if(op.mod)
        return "mod";
    else if(op.add)
        return "+";
    else if(op.sub)
        return "-";
    else if(op.sm)
        return "<";
    else if(op.sme)
        return "<=";
    else if(op.eq)
        return "=";
    else if(op.ne)
        return "<>";
    else if(op.lge)
        return ">=";
    else if(op.lg)
        return ">";
    else if(op.AND)
        return "and";
    else if(op.OR)
        return "or";
}

// Done