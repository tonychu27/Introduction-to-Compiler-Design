#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"
#include "AST/PType.hpp"
#include <iostream>
#include <string.h>

static void dumpDemarcation(const char symbol ){
    for(int i = 0 ; i < 110 ; i++) { cout << symbol; }
    puts("");
}

static void showErrorMessage(uint32_t line, uint32_t column, string errorMessage){
    cerr << "<Error> Found in line "<< line << ", column " << column << ": " << errorMessage << endl;
    cerr << "    " << code[line] << endl;
    for (int i = 1; i < column + 4; i++) cerr << " ";
    cerr << "^" << endl;
}

void SymbolEntry::dumpEntry(void){
    string levelDump;
    if(!level) levelDump = "(global)";
    else levelDump = "(local)";
    printf(OUTPUTENTRY, variableName.c_str(), kind.c_str(), level, levelDump.c_str(), type.c_str(), attribute.c_str());
}

const char *SymbolEntry::getNameCString(void) const { return variableName.c_str(); }

const char *SymbolEntry::getTypeCString(void) const { return type.c_str(); }

const char *SymbolEntry::getKindCString(void) const { return kind.c_str(); }

const char *SymbolEntry::getAttributeCString(void) const { return attribute.c_str(); }

int SymbolEntry::getTypeDimension() const{
    int dim = 0;
    for(auto c:type) { if(c == '[') dim++; }
    return dim;
}

void SymbolEntry::setKindString(const char *p_kind) { kind = p_kind; }

void SymbolEntry::setAttributeString(const char *p_attribute) { attribute = p_attribute; }

void SymbolEntry::getNewTypeDimensions(vector<uint64_t> &dims ,int ignoreBefore) const{
    int count = 0;
    for(int i = 0 ; i < type.size() ;i++){
        if(type[i]!='[') continue;
        
        if(count != ignoreBefore) {
            count++;
            continue;
        }
        
        uint64_t d = 0;
        i++;
        
        while(type[i] != ']') {
            d*=10;
            d+=type[i]-'0';
            i++;
        }
        
        dims.push_back(d);
    }
}

int SymbolEntry::getFunctionParameterCount() const{
    int num;
    if(attribute.size() == 0) return 0;
    else num = 1;

    for(auto c: attribute) { if(c == ',') num++; }
    return num;
}

void SymbolTable::addSymbol(SymbolEntry *entry) { entries.push_back(entry); }

void SymbolTable::dumpSymbol(void){
    dumpDemarcation('=');
    printf(OUTPUTFORMAT, "Name", "Kind", "Level", "Type", "Attribute");
    dumpDemarcation('-');
    for(auto it:entries) it->dumpEntry();
    dumpDemarcation('-');
}

int SymbolTable::checkRedeclaration(const char *p_name) const{
    for(auto i:entries) { if(!strcmp(i->getNameCString(), p_name)) return 1; }
    return 0;
}

void SymbolTable::addErrorSymbol(const char *p_name) { errorDeclaration.push_back(p_name); }

int SymbolTable::checkErrorDeclare(const char *p_name) const{

    for(auto err: errorDeclaration) { if(!strcmp(err, p_name)) return 1; }
    return 0;
}

SymbolEntry *SymbolTable::getEntry(const char *p_name){
    for(auto i:entries) { if(!strcmp(i->getNameCString(), p_name)) return i; }
    return NULL;
}

void SymbolManager::pushScope(SymbolTable *new_scope) { tables.push(new_scope); }

void SymbolManager::popScope(void) { tables.pop(); }

SymbolTable* SymbolManager::topScope(void) { return tables.top(); }

int SymbolManager::checkLoopVariableRedeclaration(const char *p_name){
    for(auto var:loopVar) { if(!strcmp(var, p_name)) return 1; }
    return 0;
}

int SymbolManager::checkConstant(const char *p_name){
    for(auto cons:constants) { if(!strcmp(cons, p_name)) return 1; }
    return 0;
}

void SymbolManager::pushLoopVar(const char *p_name) { loopVar.push_back(p_name); }

void SymbolManager::popLoopVar() { loopVar.pop_back(); }

void SymbolManager::pushConstant(const char *p_name) { constants.push_back(p_name); }

void SymbolManager::popConstant() { constants.pop_back(); }

int SymbolManager::getScopeSize() { return tables.size(); }

void SemanticAnalyzer::visit(ProgramNode &p_program) {

    symbolManager = new SymbolManager();
    tempManager = new SymbolManager();
    currentTable = new SymbolTable();
    currentEntry = new SymbolEntry(p_program.getNameCString(), "program", mLevel, "void", "");
    currentTable->addSymbol(currentEntry);
    symbolManager->pushScope(currentTable);

    p_program.visitChildNodes(*this);

    if(opt_dump){
        currentTable = symbolManager->topScope();
        currentTable->dumpSymbol();
    }

    symbolManager->popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) { p_decl.visitChildNodes(*this); }

void SemanticAnalyzer::visit(VariableNode &p_variable) {

    currentTable = symbolManager->topScope();
    if(currentTable->checkRedeclaration(p_variable.getNameCString()) || symbolManager->checkLoopVariableRedeclaration(p_variable.getNameCString())){
        error = 1;
        string errorMessage = "symbol '" + string(p_variable.getNameCString()) + "' is redeclared";
        showErrorMessage(p_variable.getLocation().line, p_variable.getLocation().col, errorMessage);
    }
    else {
        string type;
        
        if(flagFunction) type = "parameter";
        else if(flagFor){
            type = "loop_var";
            flagFor = 0;
            symbolManager->pushLoopVar(p_variable.getNameCString());
        }
        else type = "variable";

        currentEntry = new SymbolEntry(p_variable.getNameCString(), type.c_str(), mLevel, p_variable.getTypeCString(), "");
        currentTable->addSymbol(currentEntry);
        flagVar = 1;
        p_variable.visitChildNodes(*this);

        if(!strcmp(currentEntry->getKindCString(),"constant")) symbolManager->pushConstant(p_variable.getNameCString());
        
        flagVar = 0;
    }
    
    if(p_variable.checkInvalidDimensions()){
        error = 1;
        string errorMessage = "'" + string(p_variable.getNameCString()) + "' declared as an array with an index that is not greater than 0";
        showErrorMessage(p_variable.getLocation().line, p_variable.getLocation().col, errorMessage);
        currentTable->addErrorSymbol(p_variable.getNameCString());
    }
    
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    if(flagVar){
        currentEntry->setKindString("constant");
        currentEntry->setAttributeString(p_constant_value.getConstantValueCString());
    }
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {

    currentTable = symbolManager->topScope();
    
    if(currentTable->checkRedeclaration(p_function.getNameCString())){
        error = 1;
        string errorMessage="symbol '" + string(p_function.getNameCString()) + "' is redeclared";
        showErrorMessage(p_function.getLocation().line, p_function.getLocation().col, errorMessage);
    }
    else {
        currentEntry = new SymbolEntry(p_function.getNameCString(), "function", mLevel, p_function.getPTypeCString(), p_function.getArgumentCString());
        currentTable->addSymbol(currentEntry);
    }
    
    currentTable = new SymbolTable();
    symbolManager->pushScope(currentTable);
    mLevel++;
    flagFunction = 1;
    flagInFunction = 1;
    currentFunctionName = p_function.getNameCString();
    p_function.visitChildNodes(*this);
    currentFunctionName = "";
    flagInFunction = 0;
    flagFunction = 0;

    if(opt_dump) {
        currentTable = symbolManager->topScope();
        currentTable->dumpSymbol();
    }

    symbolManager->popScope();
    mLevel--;
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {

    bool flagPop = 1;
    if(!flagFunction){
        currentTable = new SymbolTable();
        symbolManager->pushScope(currentTable);
        mLevel++;
    }
    else {
        flagPop = 0;
        flagFunction = 0;
    }

    p_compound_statement.visitChildNodes(*this);

    if(flagPop) {

        if(opt_dump){
            currentTable = symbolManager->topScope();
            currentTable->dumpSymbol();
        }

        symbolManager->popScope();
        mLevel--;
    }
}

void SemanticAnalyzer::visit(PrintNode &p_print) {

    p_print.visitChildNodes(*this);

    if(p_print.checkInvalidChildren() == 1) return;
    
    if(p_print.checkTargetScalarType() == 0){
        error = 1;
        string errorMessage = "expression of print statement must be scalar type";
        showErrorMessage(p_print.getLocation().line, p_print.getTargetLocationCol(), errorMessage);
    }

}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {

    p_bin_op.visitChildNodes(*this);

    if(p_bin_op.checkInvalidChildren()) return;
    
    string leftType, rightType, operatorType;
    leftType = p_bin_op.getLeftTypeCString();
    rightType = p_bin_op.getRightTypeCString();
    operatorType = p_bin_op.getOpCString();
    string errorMessage = "invalid operands to binary operator '" + operatorType + "' ('" + leftType + "' and '" + rightType + "')";

    if(operatorType == "+" || operatorType == "-" || operatorType == "*" || operatorType == "/"){
        if(operatorType == "+" && leftType == "string" && rightType == "string") p_bin_op.setNodeType("string");
        else if(leftType == "integer" && rightType == "integer") p_bin_op.setNodeType("integer");
        else if(leftType == "real" && rightType == "integer") p_bin_op.setNodeType("real");
        else if(leftType == "integer" && rightType == "real") p_bin_op.setNodeType("real");
        else if(leftType == "real" && rightType == "real") p_bin_op.setNodeType("real");
        else {
            error = 1;
            showErrorMessage(p_bin_op.getLocation().line, p_bin_op.getLocation().col, errorMessage);
        }
    }
    else if(operatorType == "mod"){
        if(leftType == "integer" && rightType == "integer") p_bin_op.setNodeType("integer");
        else {
            error = 1;
            showErrorMessage(p_bin_op.getLocation().line, p_bin_op.getLocation().col, errorMessage);
        }
    }
    else if(operatorType == "and" || operatorType == "or" ){
        if(leftType == "boolean" && rightType == "boolean") p_bin_op.setNodeType("boolean");
        else {
            error = 1;
            showErrorMessage(p_bin_op.getLocation().line, p_bin_op.getLocation().col, errorMessage);
        }
    }
    else {
        if(leftType == "integer" && rightType == "integer") p_bin_op.setNodeType("boolean");
        else if(leftType == "real" && rightType == "integer") p_bin_op.setNodeType("boolean");
        else if(leftType == "integer" && rightType == "real") p_bin_op.setNodeType("boolean");
        else if(leftType == "real" && rightType == "real") p_bin_op.setNodeType("boolean");
        else {
            error = 1;
            showErrorMessage(p_bin_op.getLocation().line, p_bin_op.getLocation().col, errorMessage);
        }
    }
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);
    
    if(p_un_op.checkInvalidChildren() == 1) return;
    
    string operandType, opType, errorMessage;
    operandType = p_un_op.getOperandTypeCString();
    opType = p_un_op.getOpCString();
    
    errorMessage = "invalid operand to unary operator '" + opType + "' ('" + operandType + "')";

    if(opType == "neg"){
        if(operandType == "integer" || operandType == "real") p_un_op.setNodeType(operandType);
        else {
            error = 1;
            showErrorMessage(p_un_op.getLocation().line, p_un_op.getLocation().col, errorMessage);
        }
    }
    else {
        if(operandType == "boolean") p_un_op.setNodeType(operandType);
        else{
            error = 1;
            showErrorMessage(p_un_op.getLocation().line, p_un_op.getLocation().col, errorMessage);
        }
    }

}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {

    p_func_invocation.visitChildNodes(*this);

    while(symbolManager->getScopeSize() != 0) {

        currentTable = symbolManager->topScope();
        symbolManager->popScope();
        tempManager->pushScope(currentTable);

        if(currentTable->checkRedeclaration(p_func_invocation.getNameCString()) == 0 && symbolManager->getScopeSize() != 0) continue;
        else if(currentTable->checkRedeclaration(p_func_invocation.getNameCString()) == 0 && symbolManager->getScopeSize() == 0){
            error = 1;
            string errorMessage = "use of undeclared symbol '" + string(p_func_invocation.getNameCString()) + "'";
            showErrorMessage(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col, errorMessage); 
            break;
        }

        currentEntry = currentTable->getEntry(p_func_invocation.getNameCString());
        
        if(strcmp(currentEntry->getKindCString(), "function") != 0){
            error = 1;
            string errorMessage = "call of non-function symbol '" + string(p_func_invocation.getNameCString()) +"'";
            showErrorMessage(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col, errorMessage);
            break;
        }

        if(currentEntry->getFunctionParameterCount() != p_func_invocation.getArgumentsCount()){
            error = 1;
            string errorMessage = "too few/much arguments provided for function '" + string(p_func_invocation.getNameCString()) + "'";
            showErrorMessage(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col, errorMessage);
            break;
        }

        if(p_func_invocation.checkInvalidChildren() == 1) break;

        checkArgReturn *ret = p_func_invocation.checkArgumentsType(currentEntry->getAttributeCString());

        if(ret->wrongIdx != -1){
            error = 1;
            uint32_t wrong_col = p_func_invocation.getArgumentLocationCol(ret->wrongIdx);
            string errorMessage = "incompatible type passing '" + ret->arg + "' to parameter of type '" + ret->para +"'";
            showErrorMessage(p_func_invocation.getLocation().line, wrong_col, errorMessage);
            break;
        }

        string returnType = currentEntry->getTypeCString();
        p_func_invocation.setNodeType(returnType);

        break; 
    }

    while( tempManager->getScopeSize()!=0) {
        symbolManager->pushScope(tempManager->topScope());
        tempManager->popScope();
    }
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {

    p_variable_ref.visitChildNodes(*this);

    if(p_variable_ref.checkInvalidChildren()) return;

    currentTable = symbolManager->topScope();
    
    if(currentTable->checkErrorDeclare(p_variable_ref.getNameCString())) return;

    while(symbolManager->getScopeSize() != 0) {

        currentTable = symbolManager->topScope();
        symbolManager->popScope();
        tempManager->pushScope(currentTable);

        if(currentTable->checkRedeclaration(p_variable_ref.getNameCString())==0 && symbolManager->getScopeSize() != 0) continue;

        else if(currentTable->checkRedeclaration(p_variable_ref.getNameCString())==0 && symbolManager->getScopeSize() == 0){
            error = 1;
            string errorMessage = "use of undeclared symbol '" + string(p_variable_ref.getNameCString()) + "'";
            showErrorMessage(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, errorMessage);
            break;
        }
        currentEntry = currentTable->getEntry(p_variable_ref.getNameCString());
        if(strcmp(currentEntry->getKindCString(), "program")==0 || strcmp(currentEntry->getKindCString(), "function")==0){
            error = 1;
            string errorMessage = "use of non-variable symbol '" + string(p_variable_ref.getNameCString()) +"'";
            showErrorMessage(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, errorMessage);
            break;
        }
        int dim = p_variable_ref.getIndicesNum();
        if(dim > currentEntry->getTypeDimension()){
            error = 1;
            string errorMessage = "there is an over array subscript on '" + string(p_variable_ref.getNameCString()) + "'";
            showErrorMessage(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, errorMessage);
            break;
        }
        int nonIntCol = p_variable_ref.checkNonIntegerIndices();

        if(nonIntCol != -1){
            error = 1;
            string errorMessage = "index of array reference must be an integer";
            showErrorMessage(p_variable_ref.getLocation().line, (uint32_t)nonIntCol, errorMessage);
            break;
        }

        PTypeSharedPtr p_type;
        string primitive_type, type_string;
        type_string = currentEntry->getTypeCString();
        if(type_string.find("[") != string::npos){
            if(type_string.substr(0,4) == "void") primitive_type = "void";
            if(type_string.substr(0,7) == "boolean") primitive_type = "boolean";
            if(type_string.substr(0,6) == "string") primitive_type = "string";
            if(type_string.substr(0,7) == "integer") primitive_type = "integer";
            if(type_string.substr(0,4) == "real") primitive_type = "real";
        }
        else primitive_type = type_string;

        p_variable_ref.setNodeType(primitive_type);
        vector<uint64_t> dims;
        currentEntry->getNewTypeDimensions(dims, p_variable_ref.getIndicesNum());
        p_variable_ref.setNodeTypeDimensions(dims);
        break;
    }

    while( tempManager->getScopeSize()!=0){
        symbolManager->pushScope(tempManager->topScope());
        tempManager->popScope();
    }
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {

    p_assignment.visitChildNodes(*this);

    if(p_assignment.checkLvalue() == 1) {
        flagLoopVar = 0;
        return;
    }
    
    if(p_assignment.checkLvalueScalarType() == 0) {
        error = 1;
        string errorMessage = "array assignment is not allowed";
        showErrorMessage(p_assignment.getLocation().line, p_assignment.getLvalueLocationCol(), errorMessage);
        flagLoopVar = 0;
        return;
    }

    bool checkRval = 1;

    while(symbolManager->getScopeSize() != 0) {
        currentTable = symbolManager->topScope();
        symbolManager->popScope();
        tempManager->pushScope(currentTable);

        if(currentTable->checkRedeclaration(p_assignment.getLvalueNameCString()) == 0) continue;

        currentEntry = currentTable->getEntry(p_assignment.getLvalueNameCString());
        string lvalue_type = currentEntry->getKindCString();

        if(lvalue_type == "constant"){
            error = 1;
            string errorMessage = "cannot assign to variable '" + string(p_assignment.getLvalueNameCString()) + "' which is a constant";
            showErrorMessage(p_assignment.getLocation().line, p_assignment.getLvalueLocationCol(), errorMessage);
            checkRval = 0;
            break;
        }

        if(lvalue_type == "loop_var" && !flagLoopVar) {
            error = 1;
            string errorMessage = "the value of loop variable cannot be modified inside the loop body";
            showErrorMessage(p_assignment.getLocation().line, p_assignment.getLvalueLocationCol(), errorMessage);
            checkRval = 0;
            break;
        }
    }

    while(tempManager->getScopeSize()!=0) {
        symbolManager->pushScope(tempManager->topScope());
        tempManager->popScope();
    }

    if(p_assignment.checkRvalue()) {
        flagLoopVar = 0;
        return;
    }

    if(checkRval) {

        if(p_assignment.checkRvalueScalarType() == 0){
            error = 1;
            string errorMessage = "array assignment is not allowed";
            showErrorMessage(p_assignment.getLocation().line, p_assignment.getRvalueLocationCol(), errorMessage);
        }
        else if(p_assignment.checkCompatibleLRvalueType() == 0){
            error = 1;
            string errorMessage, lvalue_type = p_assignment.getLvalueTypeCString(), rvalue_type = p_assignment.getRvalueTypeCString();
            errorMessage = "assigning to '" + lvalue_type + "' from incompatible type '" + rvalue_type + "'";
            showErrorMessage(p_assignment.getLocation().line, p_assignment.getLocation().col, errorMessage);
        }
    }

    flagLoopVar = 0;

}

void SemanticAnalyzer::visit(ReadNode &p_read) {

    p_read.visitChildNodes(*this);

    if(p_read.checkInvalidChildren() == 1) return;

    if(p_read.checkTargetScalarType() == 0){
        error = 1;
        string errorMessage = "variable reference of read statement must be scalar type";
        showErrorMessage(p_read.getLocation().line, p_read.getTargetLocationCol(), errorMessage);
    }

    if(symbolManager->checkConstant(p_read.getTargetNameCString()) || symbolManager->checkLoopVariableRedeclaration(p_read.getTargetNameCString())){
        error = 1;
        string errorMessage = "variable reference of read statement cannot be a constant or loop variable";
        showErrorMessage(p_read.getLocation().line, p_read.getTargetLocationCol(), errorMessage);
    }

}

void SemanticAnalyzer::visit(IfNode &p_if) {

    p_if.visitChildNodes(*this);

    if(p_if.checkInvalidCondition() == 1) return;

    if(p_if.checkConditionBoolType() == 0){
        error = 1;
        string errorMessage = "the expression of condition must be boolean type";
        showErrorMessage(p_if.getLocation().line, p_if.getConditionLocationCol(), errorMessage);
    }

}

void SemanticAnalyzer::visit(WhileNode &p_while) {

    p_while.visitChildNodes(*this);

    if(p_while.checkInvalidCondition() == 1) return;

    if(p_while.checkConditionBoolType() == 0){
        error = 1;
        string errorMessage = "the expression of condition must be boolean type";
        showErrorMessage(p_while.getLocation().line, p_while.getLocation().col, errorMessage);
    }
}

void SemanticAnalyzer::visit(ForNode &p_for) {

    currentTable = new SymbolTable();
    symbolManager->pushScope(currentTable);
    mLevel++;
    flagFor = 1;
    flagLoopVar = 1;

    p_for.visitChildNodes(*this);

    flagLoopVar = 0;
    flagFor = 0;

    int init = atoi(p_for.getInitialValueCString());
    int condition = atoi(p_for.getConditionValueCString());
    
    if(init > condition){
        error = 1;
        string errorMessage = "the lower bound and upper bound of iteration count must be in the incremental order";
        showErrorMessage(p_for.getLocation().line, p_for.getLocation().col, errorMessage);
    }
    
    symbolManager->popLoopVar();
    
    if(opt_dump) {
        currentTable = symbolManager->topScope();
        currentTable->dumpSymbol();
    }

    symbolManager->popScope();
    mLevel--;
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {

    p_return.visitChildNodes(*this);

    if(!flagInFunction){
        error = 1;
        string errorMessage = "program/procedure should not return a value";
        showErrorMessage(p_return.getLocation().line, p_return.getLocation().col, errorMessage);
        return;
    }

    while(symbolManager->getScopeSize() != 0){
        currentTable = symbolManager->topScope();
        symbolManager->popScope();
        tempManager->pushScope(currentTable);

        if(currentTable->checkRedeclaration(currentFunctionName.c_str()) == 0) continue;

        SymbolEntry *function_entry = currentTable->getEntry(currentFunctionName.c_str());
        string returnType = function_entry->getTypeCString();
        if(returnType == "void"){
            error = 1;
            string errorMessage = "program/procedure should not return a value";
            showErrorMessage(p_return.getLocation().line, p_return.getLocation().col, errorMessage);
            break;
        }

        if(p_return.checkInvalidRetrunType() == 1) break;
        
        string return_value_type = p_return.getReturnTypeCString();
        if(return_value_type != returnType){
            error = 1;
            string errorMessage = "return '" + return_value_type + "' from a function with return type '" + returnType + "'";
            showErrorMessage(p_return.getLocation().line, p_return.getReturnLocationCol(), errorMessage);
            break; 
        }
    }

    while(tempManager->getScopeSize()!=0){
        symbolManager->pushScope(tempManager->topScope());
        tempManager->popScope();
    }
    
}
