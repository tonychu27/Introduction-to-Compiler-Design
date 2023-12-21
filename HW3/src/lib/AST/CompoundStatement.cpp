#include "AST/CompoundStatement.hpp"

CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, vector<AstNode*> *p_declList, vector<AstNode*> *p_stateList)
    : AstNode{line, col}, declList(p_declList), stateList(p_stateList) {}

void CompoundStatementNode::print() { printf(" Compound Statement <line: %u, col: %u>\n", location.line, location.col); }

void CompoundStatementNode::accept(AstNodeVisitor &p_visitor){ p_visitor.visit(*this); }

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {

    if(declList) { for(auto &d: *declList){ d->accept(p_visitor); } }
    if(stateList) { for(auto &s: *stateList) { s->accept(p_visitor); } }
}

int CompoundStatementNode::getDeclListLen() { return declList->size(); }

int CompoundStatementNode::getStateListLen() { return stateList->size(); }

// Done