#include "AST/ConstantValue.hpp"
#include <string.h>

char r[1024];

ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, ConstValue p_constVal)
    : ExpressionNode{line, col}, constVal(p_constVal) {}

void ConstantValueNode::print() { printf(" Constant Value <line: %u, col: %u>\n", location.line, location.col); }

void ConstantValueNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

const char* ConstantValueNode::getCstringValue() const{

    memset(r, 0, sizeof(r));

    if(constVal.boolType) { return constVal.strValue; }
    if(constVal.realType) { sprintf(r, "%f",constVal.realValue); }
    else if(constVal.intType) { sprintf(r, "%d",constVal.intValue); }
    else if(constVal.strType) { return constVal.strValue; }
 
    return r;
}

// Done