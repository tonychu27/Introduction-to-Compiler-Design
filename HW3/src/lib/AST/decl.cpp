#include "AST/decl.hpp"

DeclNode::DeclNode(const uint32_t line, const uint32_t col, vector<AstNode*> *p_varList)
    : AstNode{line, col}, varList(p_varList) {}

void DeclNode::print() { printf("Decl <line: %u, col: %u>\n", location.line, location.col); }

void DeclNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) { if(varList) { for(auto &v: *varList) { v->accept(p_visitor); } } }

vector<AstNode*> *DeclNode::getVarList() { return varList; }
// Done