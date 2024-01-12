#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <string>

CodeGenerator::CodeGenerator(const char *in_file_name, const char *out_file_name, SymbolManager *m_symbol_manager) 
    : in_file_name(in_file_name), m_symbol_manager(m_symbol_manager) {
    this->m_output_file = fopen(out_file_name, "w");
    assert(this->m_output_file != NULL && "fopen() fails");
}

void CodeGenerator::dumpInstruction(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(this->m_output_file, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    
    m_symbol_manager->reconstructHashTableFromSymbolTable(p_program.getSymbolTable());

    
    dumpInstruction("%s%s%s%s", 
        "   .file \"", this->in_file_name, "\"\n",
        "   .option nopic\n"
    );


    for (auto &decl : p_program.var_decls) { decl->accept(*this); }
    for (auto &func : p_program.funcs) { func->accept(*this); }

    dumpInstruction("\n.text\n");
    dumpInstruction("  .align  2\n");
    dumpInstruction("  .global main\n");
    dumpInstruction("  .type   main, @function\n");
    dumpInstruction("\nmain:\n");

    offset = 0;
    offset -=  8;
    dumpInstruction("    addi sp, sp, -128\n");   
    dumpInstruction("    sw ra, 124(sp)\n");       
    dumpInstruction("    sw s0, 120(sp)\n");       
    dumpInstruction("    addi s0, sp, 128\n");  
    p_program.body->accept(*this);
    dumpInstruction("    lw ra, 124(sp)    # load return address saved in the current stack\n");        
    dumpInstruction("    lw s0, 120(sp)    # move frame pointer back to the bottom of the last stack\n");       
    dumpInstruction("    addi sp, sp, 128  # move stack pointer back to the top of the last stack\n");   
    dumpInstruction("    jr ra             # jump back to the caller function\n");                          
    dumpInstruction("    .size %s, .-%s\n", "main", "main");
    offset += 8;


    m_symbol_manager->removeSymbolsFromHashTable(p_program.getSymbolTable());
}

void CodeGenerator::visit(DeclNode &p_decl) { p_decl.visitChildNodes(*this); }

void CodeGenerator::visit(VariableNode &p_variable) {
    if(!this->in_local){
        if(m_symbol_manager->lookup(p_variable.getName())->getKind() == SymbolEntry::KindEnum::kVariableKind){ dumpInstruction("\n.comm %s, 4, 4\n", p_variable.getNameCString());}
        else if(m_symbol_manager->lookup(p_variable.getName())->getKind() == SymbolEntry::KindEnum::kConstantKind){
            dumpInstruction("\n.section    .rodata \n");
            dumpInstruction("    .align 2\n");
            dumpInstruction("    .globl %s\n", p_variable.getNameCString());
            dumpInstruction("    .type %s, @object\n", p_variable.getNameCString());
            dumpInstruction("%s:\n", p_variable.getNameCString());
            dumpInstruction("    .word %s \n", p_variable.getConstantPtr()->getConstantValueCString() );
        }
    }
    else{
        SymbolEntry *DeclEntry = (SymbolEntry *) m_symbol_manager->lookup(p_variable.getName());
        if(DeclEntry->getKind() == SymbolEntry::KindEnum::kVariableKind){
            offset -=  4;
            DeclEntry->set_offset(offset);
            dumpInstruction("# Declare local const variable:%s, offset: %d\n", DeclEntry->getNameCString(), DeclEntry->get_offset());
        }
        else if(DeclEntry->getKind() == SymbolEntry::KindEnum::kConstantKind){
            offset -=  4;
            DeclEntry->set_offset(offset);
            dumpInstruction("# Declare local const variable:%s, offset: %d\n", DeclEntry->getNameCString(), DeclEntry->get_offset());
            dumpInstruction("    addi t0, s0, %d\n", offset);
            dumpInstruction("    addi sp, sp, -4\n");
            dumpInstruction("    sw t0, 0(sp)\n");
            dumpInstruction("    li t0, %s\n", p_variable.getConstantPtr()->getConstantValueCString());
            dumpInstruction("    addi sp, sp, -4\n");
            dumpInstruction("    sw t0, 0(sp)\n");
            dumpInstruction("    lw t0, 0(sp)    # pop the value from the stack\n");
            dumpInstruction("    addi sp, sp, 4\n");
            dumpInstruction("    lw t1, 0(sp)    # pop the address from the stack\n");
            dumpInstruction("    addi sp, sp, 4\n");
            dumpInstruction("    sw t0, 0(t1)\n");
        }
        else if(DeclEntry->getKind() == SymbolEntry::KindEnum::kParameterKind){
            offset -=  4;
            DeclEntry->set_offset(offset);
        }
        else if(DeclEntry->getKind() == SymbolEntry::KindEnum::kLoopVarKind){
            offset -=  4;
            DeclEntry->set_offset(offset);
        }
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    if(p_constant_value.getTypePtr()->isInteger()){
        dumpInstruction("    li t0, %s\n", p_constant_value.getConstantValueCString());
        dumpInstruction("    addi sp, sp, -4\n");
        dumpInstruction("    sw t0, 0(sp)\n");
    }
    else if(p_constant_value.getTypePtr()->isBool()){
        if(p_constant_value.getConstantValueCString() == std::string("true"))
            dumpInstruction("    li t0, 1\n");
        else
            dumpInstruction("    li t0, 0\n");
        dumpInstruction("    addi sp, sp, -4\n");
        dumpInstruction("    sw t0, 0(sp)\n");
    }
}

void CodeGenerator::visit(FunctionNode &p_function) {
    
    m_symbol_manager->reconstructHashTableFromSymbolTable(p_function.getSymbolTable());
    in_local = 1;
    

    dumpInstruction("\n.text\n");
    dumpInstruction("  .align  2\n");
    dumpInstruction("  .global %s\n", p_function.getNameCString());
    dumpInstruction("  .type   %s, @function\n", p_function.getNameCString());
    dumpInstruction("\n%s:\n", p_function.getNameCString());

    offset = 0;
    offset -=  8;
    dumpInstruction("    addi sp, sp, -128    # move stack pointer to lower address to allocate a new stack\n");   
    dumpInstruction("    sw ra, 124(sp)       # save return address of the caller function in the current stack\n");       
    dumpInstruction("    sw s0, 120(sp)       # save frame pointer of the last stack in the current stack\n");       
    dumpInstruction("    addi s0, sp, 128     # move frame pointer to the bottom of the current stack\n");  

    dumpInstruction("# Parameter num: %d\n", p_function.getParametersNum_t());
    for (auto &parameter : p_function.parameters) { parameter->accept(*this);}
    if(p_function.getParametersNum_t() <= 8){
        for(int i = 0; i < p_function.getParametersNum_t(); i++){
            std::string para_name = p_function.getSymbolTable()->getEntries().at(i)->getName();
            SymbolEntry *ParaEntry = (SymbolEntry *) m_symbol_manager->lookup(para_name);
            dumpInstruction("    sw a%d, %d(s0)\n", i, ParaEntry->get_offset());
        }
    }
    else{
        int super_8_size = 4 * p_function.getParametersNum_t();
        for(int i = 0; i < p_function.getParametersNum_t(); i++){
            std::string para_name = p_function.getSymbolTable()->getEntries().at(i)->getName();
            SymbolEntry *ParaEntry = (SymbolEntry *) m_symbol_manager->lookup(para_name);
            dumpInstruction("    lw t0, %d(s0)\n", super_8_size - 4);//load parameter from FP
            dumpInstruction("    sw t0, %d(s0)\n", ParaEntry->get_offset());
            super_8_size -= 4;
        }
    }
    if (p_function.body) { p_function.body->accept(*this); }

    dumpInstruction("    lw ra, 124(sp)    # load return address saved in the current stack\n");        
    dumpInstruction("    lw s0, 120(sp)    # move frame pointer back to the bottom of the last stack\n");       
    dumpInstruction("    addi sp, sp, 128  # move stack pointer back to the top of the last stack\n");   
    dumpInstruction("    jr ra             # jump back to the caller function\n");                          
    dumpInstruction("    .size %s, .-%s\n", p_function.getNameCString(), p_function.getNameCString());
    offset += 8;

    in_local = 0;
    m_symbol_manager->removeSymbolsFromHashTable(p_function.getSymbolTable());
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {

    m_symbol_manager->reconstructHashTableFromSymbolTable(p_compound_statement.getSymbolTable());
    in_local = 1;

    p_compound_statement.visitChildNodes(*this);

    m_symbol_manager->removeSymbolsFromHashTable(p_compound_statement.getSymbolTable());
}

void CodeGenerator::visit(PrintNode &p_print) {
    dumpInstruction("# Print start\n");
    p_print.visitChildNodes(*this);
    
    dumpInstruction("    lw a0, 0(sp)\n");
    dumpInstruction("    addi sp, sp, 4\n");
    dumpInstruction("    jal ra, printInt\n");
    dumpInstruction("# Print end\n");
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    p_bin_op.visitChildNodes(*this);
    std::string op(p_bin_op.getOpCString());
    dumpInstruction("    lw t0, 0(sp)\n");
    dumpInstruction("    addi sp, sp, 4\n");
    dumpInstruction("    lw t1, 0(sp)\n");
    dumpInstruction("    addi sp, sp, 4\n");
    if(op == "+") { dumpInstruction("    add t0, t1, t0 \n"); }
    else if(op == "-") { dumpInstruction("    sub t0, t1, t0 \n"); }
    else if(op == "*") { dumpInstruction("    mul t0, t1, t0 \n"); }
    else if(op == "/") { dumpInstruction("    div t0, t1, t0 \n"); }
    else if(op == "mod") { dumpInstruction("    rem t0, t1, t0 \n"); }
    else if(op == "<"){
        int label_out = label++;
        int label_true = label++;
        dumpInstruction("    blt t1, t0, L%d\n", label_true);
        dumpInstruction("    li t0, 0\n");
        dumpInstruction("    j L%d\n", label_out);
        dumpInstruction("L%d:\n", label_true);
        dumpInstruction("    li t0, 1\n");
        dumpInstruction("L%d:\n", label_out);
    }
    else if(op == "<="){
        int label_out = label++;
        int label_true = label++;
        dumpInstruction("    ble t1, t0, L%d\n", label_true);
        dumpInstruction("    li t0, 0\n");
        dumpInstruction("    j L%d\n", label_out);
        dumpInstruction("L%d:\n", label_true);
        dumpInstruction("    li t0, 1\n");
        dumpInstruction("L%d:\n", label_out);
    }
    else if(op == "="){
        int label_out = label++;
        int label_true = label++;
        dumpInstruction("    beq t0, t1, L%d\n", label_true);
        dumpInstruction("    li t0, 0\n");
        dumpInstruction("    j L%d\n", label_out);
        dumpInstruction("L%d:\n", label_true);
        dumpInstruction("    li t0, 1\n");
        dumpInstruction("L%d:\n", label_out);
    }
    else if(op == ">="){
        int label_out = label++;
        int label_true = label++;
        dumpInstruction("    bge t1, t0, L%d\n", label_true);
        dumpInstruction("    li t0, 0\n");
        dumpInstruction("    j L%d\n", label_out);
        dumpInstruction("L%d:\n", label_true);
        dumpInstruction("    li t0, 1\n");
        dumpInstruction("L%d:\n", label_out);
    }
    else if(op == ">"){
        int label_out = label++;
        int label_true = label++;
        dumpInstruction("    bgt t1, t0, L%d\n", label_true);
        dumpInstruction("    li t0, 0\n");
        dumpInstruction("    j L%d\n", label_out);
        dumpInstruction("L%d:\n", label_true);
        dumpInstruction("    li t0, 1\n");
        dumpInstruction("L%d:\n", label_out);
    }
    else if(op == "<>"){
        int label_out = label++;
        int label_true = label++;
        dumpInstruction("    bne t0, t1, L%d\n", label_true);
        dumpInstruction("    li t0, 0\n");
        dumpInstruction("    j L%d\n", label_out);
        dumpInstruction("L%d:\n", label_true);
        dumpInstruction("    li t0, 1\n");
        dumpInstruction("L%d:\n", label_out);
    } 
    else if(op == "and"){
        int label_out = label++;
        int label_false = label++;
        dumpInstruction("    beqz t1, L%d\n", label_false);

        dumpInstruction("    beqz t0, L%d\n", label_false);

        dumpInstruction("    li t0, 1\n");
        dumpInstruction("    j L%d\n", label_out);

        dumpInstruction("L%d:\n", label_false);
        dumpInstruction("    li t0, 0\n");

        dumpInstruction("L%d:\n", label_out);
    }
    else if(op == "or"){
        int label_out = label++;
        int label_true = label++;
        dumpInstruction("    bneqz t1, L%d\n", label_true);

        dumpInstruction("    bneqz t0, L%d\n", label_true);

        dumpInstruction("    li t0, 0\n");
        dumpInstruction("    j L%d\n", label_out);

        dumpInstruction("L%d:\n", label_true);
        dumpInstruction("    li t0, 1\n");

        dumpInstruction("L%d:\n", label_out);
    }

    dumpInstruction("    addi sp, sp, -4\n");
    dumpInstruction("    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);
    std::string op(p_un_op.getOpCString());
    dumpInstruction("    lw t0, 0(sp)\n");
    dumpInstruction("    addi sp, sp, 4\n");
    if(op == "neg"){
        dumpInstruction("    li t1, 0\n");
        dumpInstruction("    sub t0, t1, t0 \n");
    }
    else if(op == "not"){
        dumpInstruction("    li t1, 1\n");
        dumpInstruction("    sub t0, t1, t0 \n");
    }
    dumpInstruction("    addi sp, sp, -4\n");
    dumpInstruction("    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    dumpInstruction("# Function call start\n");
    p_func_invocation.visitChildNodes(*this);
    int argc = p_func_invocation.get_argument_num();
    int super_8_size = 0;
    if(argc <= 8){
        for(int i = argc-1; i >= 0; i--){
            dumpInstruction("    lw a%d, 0(sp)\n", i);
            dumpInstruction("    addi sp, sp, 4\n");
        }
    }
    else { super_8_size = argc * 4; }
    dumpInstruction("    jal ra, %s\n", p_func_invocation.getNameCString());

    if(super_8_size > 0) { dumpInstruction("    addi sp, sp, %d\n", super_8_size); }
    dumpInstruction("    mv t0, a0\n");
    dumpInstruction("    addi sp, sp, -4\n");
    dumpInstruction("    sw t0, 0(sp)\n");
    dumpInstruction("# Function call end\n");
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    p_variable_ref.visitChildNodes(*this);
    SymbolEntry* Variable_entry = (SymbolEntry*)m_symbol_manager->lookup(p_variable_ref.getName());
    dumpInstruction("# Variable reference:%s, offset: %d\n", Variable_entry->getNameCString(), Variable_entry->get_offset());
    if(this->is_read == 1){
        if(Variable_entry->getLevel() == 0){
            dumpInstruction("    la t0, %s\n", p_variable_ref.getNameCString());
            dumpInstruction("    addi sp, sp, -4\n");
            dumpInstruction("    sw t0, 0(sp)\n");
        }
        else{
            dumpInstruction("    addi t0, s0, %d\n", Variable_entry->get_offset() );
            dumpInstruction("    addi sp, sp, -4\n", p_variable_ref.getNameCString());
            dumpInstruction("    sw t0, 0(sp)\n");
        }
    }
    else{
        if(Variable_entry->getLevel() == 0){
            dumpInstruction("    la t0, %s\n", p_variable_ref.getNameCString());
            dumpInstruction("    lw t1, 0(t0)\n");
            dumpInstruction("    mv t0, t1\n");
            dumpInstruction("    addi sp, sp, -4\n");
            dumpInstruction("    sw t0, 0(sp)\n");
        }
        else{
            dumpInstruction("    lw t0, %d(s0)\n", Variable_entry->get_offset());
            dumpInstruction("    addi sp, sp, -4 \n");
            dumpInstruction("    sw t0, 0(sp)\n");
        }
    }
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    dumpInstruction("# Assignment start\n");
    is_read = 1;
    p_assignment.lvalue->accept(*this);
    is_read = 0;
    p_assignment.expr->accept(*this);

    dumpInstruction("    lw t0, 0(sp)\n");
    dumpInstruction("    addi sp, sp, 4\n");
    dumpInstruction("    lw t1, 0(sp)\n");
    dumpInstruction("    addi sp, sp, 4\n");
    dumpInstruction("    sw t0, 0(t1)\n");
    dumpInstruction("# Assignment end\n");
}

void CodeGenerator::visit(ReadNode &p_read) {
    dumpInstruction("# Read start\n");
    is_read = 1;
    p_read.visitChildNodes(*this);
    is_read = 0;
    dumpInstruction("    jal ra, readInt\n");
    dumpInstruction("    lw t0, 0(sp)\n");
    dumpInstruction("    addi sp, sp, 4\n");
    dumpInstruction("    sw a0, 0(t0)\n");
    dumpInstruction("# Read end\n");
}

void CodeGenerator::visit(IfNode &p_if) {  
    int if_label = label++;
    int else_label = label++;
    int out_label = label++;

    p_if.condition->accept(*this);

    dumpInstruction("    lw t0, 0(sp)\n");
    dumpInstruction("    addi sp, sp, 4\n");
    dumpInstruction("    beqz t0, L%d\n", else_label);

    dumpInstruction("# Label for if\n");
    dumpInstruction("L%d:\n", if_label);
    p_if.body->accept(*this);
    dumpInstruction("    j L%d\n", out_label);

    dumpInstruction("# Label for else\n");
    dumpInstruction("L%d:\n", else_label);
    if (p_if.else_body) { p_if.else_body->accept(*this); }
    dumpInstruction("# Label for out\n");
    dumpInstruction("L%d:\n", out_label);
}

void CodeGenerator::visit(WhileNode &p_while) {
    int loop_label = label++;
    int out_label = label++;

    dumpInstruction("L%d:\n", loop_label);
    p_while.condition->accept(*this);
    dumpInstruction("    lw t0, 0(sp)\n");
    dumpInstruction("    addi sp, sp, 4\n");
    dumpInstruction("    beqz t0, L%d\n", out_label);
    p_while.body->accept(*this);
    dumpInstruction("    j L%d\n", loop_label);

    dumpInstruction("L%d:\n", out_label);
}

void CodeGenerator::visit(ForNode &p_for) {

    m_symbol_manager->reconstructHashTableFromSymbolTable(p_for.getSymbolTable());

    SymbolEntry* loop_var_entry = (SymbolEntry *) m_symbol_manager->lookup(p_for.var_decl->get_first_name_Cstr());
    dumpInstruction("# For start\n");
    dumpInstruction("# For_var name: %s\n", loop_var_entry->getNameCString());
    int loop_label = label++;
    int out_label = label++;

    p_for.var_decl->accept(*this);
    p_for.initial_statement->accept(*this);

    dumpInstruction("L%d:\n", loop_label);
    p_for.condition->accept(*this);
    dumpInstruction("    lw t0, 0(sp)\n");
    dumpInstruction("    addi sp, sp, 4\n");

    dumpInstruction("    lw t1, %d(s0)\n", loop_var_entry->get_offset());
    dumpInstruction("    bge t1, t0, L%d\n", out_label);

    p_for.body->accept(*this);

    dumpInstruction("    lw t1, %d(s0)\n", loop_var_entry->get_offset());
    dumpInstruction("    addi t1, t1, 1\n");
    dumpInstruction("    sw t1, %d(s0)\n", loop_var_entry->get_offset());
    dumpInstruction("    j L%d\n", loop_label);

    dumpInstruction("# For end\n");
    dumpInstruction("L%d:\n", out_label);
    
    m_symbol_manager->removeSymbolsFromHashTable(p_for.getSymbolTable());
}


void CodeGenerator::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);
    dumpInstruction("# Return start\n");
    dumpInstruction("    lw t0, 0(sp)\n");
    dumpInstruction("    addi sp, sp, 4\n");
    dumpInstruction("    mv a0, t0\n");
    dumpInstruction("# Return end\n");
}

