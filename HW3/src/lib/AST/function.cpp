#include "AST/function.hpp"

FunctionNode::FunctionNode(const uint32_t line, const uint32_t col, const char *p_name, const char *p_renType, vector<AstNode*> *p_declList, AstNode* p_compound)
    : AstNode{line, col}, name(p_name), renType(p_renType), declList(p_declList), compound(p_compound) {}

void FunctionNode::print() { printf("Function <line: %u, col: %u>\n", location.line, location.col); }

void FunctionNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(declList) { for(auto &d: *declList) { d->accept(p_visitor); } }

    if(compound) { compound->accept(p_visitor); }
}

const char *FunctionNode::getNameCStirng() const{ return name.c_str(); }

const char *FunctionNode::getReturnTypeCString() const{return renType.c_str(); }

string FunctionNode::getPrototypeString() {

    string buf = "(";

    VariableNode *var;
    DeclNode *decl;

    if(declList) {
        int cnt = 0;

        for (auto& d: *declList) {

            decl = dynamic_cast<DeclNode*>(d);
            vector<AstNode*> *varLis = decl->getVarList();

            if(cnt) { buf += ", "; }
            
            if(varLis){

                size_t c = 0;
                for (auto& v: *varLis) {

                    var = dynamic_cast<VariableNode*>(v);
                    buf += var->getType();
                    if(c != varLis->size() - 1) { buf += ", "; }

                    c++;
                }
            }

            cnt++;
        }
    }

    buf +=")";

    return buf;
}

// Done