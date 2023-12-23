#include "AST/expression.hpp"

const char *ExpressionNode::getPTypeCString(){
    if(type != nullptr)
        return type->getPTypeCString();
    else
        return "null";
}

void ExpressionNode::setNodeType(string type_string){
    if(type_string == "void")
        type = make_shared<PType>(PType::PrimitiveTypeEnum::kVoidType);
    else if(type_string == "boolean")
       type = make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType);
    else if(type_string == "string")
        type = make_shared<PType>(PType::PrimitiveTypeEnum::kStringType);
    else if(type_string == "integer")
        type = make_shared<PType>(PType::PrimitiveTypeEnum::kIntegerType);
    else if(type_string == "real")
        type = make_shared<PType>(PType::PrimitiveTypeEnum::kRealType);
}

void ExpressionNode::setNodeTypeDimensions(vector<uint64_t> dims){
    type->setDimensions(dims);
}