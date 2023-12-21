#include "AST/variable.hpp"

VariableNode::VariableNode(const uint32_t line, const uint32_t col, const char *p_name, const char *p_type, AstNode *p_constant)
    : AstNode{line, col}, name(p_name), type(p_type), constant(p_constant) {}


void VariableNode::print() { printf("Variable <line: %u, col: %u>\n", location.line, location.col); }

void VariableNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) { if(constant != NULL) constant->accept(p_visitor); }

string VariableNode::getType() { return type; }

const char *VariableNode::getTypeCString() { return type.c_str(); }

const char *VariableNode::getNameCString() { return name.c_str(); }
