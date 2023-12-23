#include "AST/assignment.hpp"
#include "AST/ConstantValue.hpp"

const char *AssignmentNode::getConstantValueCString() {
    unique_ptr<ConstantValueNode> expr_const(dynamic_cast<ConstantValueNode *>(mExpr.get()));
    if(expr_const) { mExpr.release(); }

    return expr_const->getConstantValueCString();
}

const char *AssignmentNode::getLvalueNameCString() { return mLvalue->getNameCString(); }

const char *AssignmentNode::getLvalueTypeCString() { return mLvalue->getPTypeCString(); }

const char *AssignmentNode::getRvalueTypeCString() { return mExpr->getPTypeCString(); }

const uint32_t AssignmentNode::getLvalueLocationCol() const { return mLvalue->getLocation().col; }

const uint32_t AssignmentNode::getRvalueLocationCol() const { return mExpr->getLocation().col; }

const int AssignmentNode::checkLvalue() const {
    string lvalue = mLvalue->getPTypeCString();
    if(lvalue == "null") return 1;
    return 0; 
}

const int AssignmentNode::checkRvalue() const {
    string rvalue = mExpr->getPTypeCString();
    if(rvalue == "null") return 1;
    return 0;
}

const int AssignmentNode::checkLvalueScalarType() const{
    string lvalue = mLvalue->getPTypeCString();
    if(lvalue == "integer" || lvalue == "real" || lvalue == "string" || lvalue == "boolean") return 1;
    return 0;
}

const int AssignmentNode::checkRvalueScalarType() const{
    string rvalue = mExpr->getPTypeCString();
    if(rvalue == "integer" || rvalue == "real" || rvalue == "string" || rvalue == "boolean") return 1;
    return 0;
}

const int AssignmentNode::checkCompatibleLRvalueType() const{
    string lvalue = mLvalue->getPTypeCString(), rvalue = mExpr->getPTypeCString();
    if(lvalue == rvalue) return 1;
    else if(lvalue == "real" && rvalue == "integer") return 1;
    return 0;
}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    mLvalue->accept(p_visitor);
    mExpr->accept(p_visitor);
}
