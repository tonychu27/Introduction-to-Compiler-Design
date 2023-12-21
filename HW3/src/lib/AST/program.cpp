#include "AST/program.hpp"

ProgramNode::ProgramNode(const uint32_t line, const uint32_t col, const char *p_name, const char*p_renType, vector<AstNode*> *p_declList, vector<AstNode*> *p_funcList, AstNode* p_compound)
    : AstNode{line, col}, name(p_name), renType(p_renType), declList(p_declList), funcList(p_funcList), compound(p_compound) {}

const char *ProgramNode::getNameCString() const{return name.c_str();} 

void ProgramNode::print() {

    printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");
}

void ProgramNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) {

    if(declList) { for(auto &d : *declList){ d->accept(p_visitor); } }
    if(funcList) { for(auto &f : *funcList) { f->accept(p_visitor); } }
    if(compound != NULL) { compound->accept(p_visitor); }
}
// Done