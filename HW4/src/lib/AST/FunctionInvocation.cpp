#include "AST/FunctionInvocation.hpp"

#include <algorithm>
#include <string.h>

const int FunctionInvocationNode::getArgumentsCount() const { return m_args.size(); }

const uint32_t FunctionInvocationNode::getArgumentLocationCol(const int id) const { return m_args[id]->getLocation().col; }

const char *FunctionInvocationNode::getArgumentTypeCString(const int id) const { return m_args[id]->getPTypeCString(); }

const int FunctionInvocationNode::checkInvalidChildren() const{
    for(auto &i:m_args) { if(!strcmp(i->getPTypeCString(), "null")) return 1; }
    return 0;
}

checkArgReturn *FunctionInvocationNode::checkArgumentsType(const char *attribute) const{
    
    string attriString(attribute);
    checkArgReturn *ret = new checkArgReturn;
    bool flagWrong = false;
    size_t left = 0, right = attriString.find(',');

    for(int i = 0 ; i < m_args.size() ; i++){
        if(right == string::npos)
            right = attriString.size();
        string arg = m_args[i]->getPTypeCString(), para = attriString.substr(left, right-left);
        
        if(para == "real"){
            if(arg != "real" && arg != "integer"){
                ret->wrongIdx = i;
                ret->para = para;
                ret->arg = arg;
                flagWrong = true;
                break;
            }
        }
        else if(para != arg){
            ret->wrongIdx = i;
            ret->para = para;
            ret->arg = arg;
            flagWrong = true;
            break;
        }
        if(i != m_args.size()-1){
            attriString = attriString.substr(right+2);
            right = attriString.find(',');
        }
    }
    if(!flagWrong) ret->wrongIdx = -1;

    return ret;
}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(p_visitor); };

    for_each(m_args.begin(), m_args.end(), visit_ast_node);
}
