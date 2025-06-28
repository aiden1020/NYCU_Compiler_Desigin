#include "AST/CompoundStatement.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/program.hpp"
#include "codegen/CodeGenerator.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

CodeGenerator::CodeGenerator(const std::string &source_file_name,
                             const std::string &save_path,
                             std::unordered_map<SemanticAnalyzer::AstNodeAddr,
                                                SymbolManager::Table>
                                 &&p_symbol_table_of_scoping_nodes)
    : m_symbol_manager(false /* no dump */),
      m_source_file_path(source_file_name),
      m_symbol_table_of_scoping_nodes(std::move(p_symbol_table_of_scoping_nodes))
{
    // FIXME: assume that the source file is always xxxx.p
    const auto &real_path =
        save_path.empty() ? std::string{"."} : save_path;
    auto slash_pos = source_file_name.rfind('/');
    auto dot_pos = source_file_name.rfind('.');

    if (slash_pos != std::string::npos)
    {
        ++slash_pos;
    }
    else
    {
        slash_pos = 0;
    }
    auto output_file_path{
        real_path + "/" +
        source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S"};
    m_output_file.reset(fopen(output_file_path.c_str(), "w"));
    assert(m_output_file.get() && "Failed to open output file");
}

static void dumpInstructions(FILE *p_out_file, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(p_out_file, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program)
{
    // Generate RISC-V instructions for program header
    // clang-format off
    constexpr const char *const riscv_assembly_file_prologue =
        "    .file \"%s\"\n"
        "    .option nopic\n";

    constexpr const char *riscv_assembly_main_start =
        ".section    .text\n"
        "    .align 2\n"
        "    .globl main\n"
        "    .type main, @function\n"
        "main:\n";
    constexpr const char *riscv_assembly_main_end =
        "    .size main, .-main\n";
    constexpr const char *const riscv_assembly_prologue = "    addi sp, sp, -128\n"
                                                      "    sw ra, 124(sp)\n"
                                                      "    sw s0, 120(sp)\n"
                                                      "    addi s0, sp, 128\n\n";
    constexpr const char *const riscv_assembly_epilogue = "    lw ra, 124(sp)\n"
                                                      "    lw s0, 120(sp)\n"
                                                      "    addi sp, sp, 128\n"
                                                      "    jr ra\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
                     m_source_file_path.c_str());

    // Reconstruct the scope for looking up the symbol entry.
    // Hint: Use m_symbol_manager->lookup(symbol_name) to get the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_program)));

    auto visit_ast_node = [&](auto &ast_node)
    { ast_node->accept(*this); };
    for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
             visit_ast_node);
    for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
             visit_ast_node);

    dumpInstructions(m_output_file.get(), riscv_assembly_main_start);
    dumpInstructions(m_output_file.get(), riscv_assembly_prologue);
    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);
    dumpInstructions(m_output_file.get(), riscv_assembly_epilogue);
    dumpInstructions(m_output_file.get(), riscv_assembly_main_end);

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(DeclNode &p_decl)
{
    m_is_in_declaration = true;
    p_decl.visitChildNodes(*this);
    m_is_in_declaration = false;
}

void CodeGenerator::visit(VariableNode &p_variable)
{
    SymbolEntry *symbol_entry = const_cast<SymbolEntry *>(m_symbol_manager.lookup(p_variable.getName()));
    if (!symbol_entry)
        return;

    const char *variable_name = symbol_entry->getNameCString();
    if (symbol_entry->getTypePtr()->getPrimitiveType() == PType::PrimitiveTypeEnum::kBoolType)
        m_assign_bool = true;
    if (m_is_in_declaration)
    {
        if (symbol_entry->getLevel() == 0)
        { // Global Declaration
            if (p_variable.getConstantPtr() == nullptr)
            {
                constexpr const char *const riscv_assembly_GlobalVarDecl = ".comm %s, 4, 4\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_GlobalVarDecl, variable_name);
            }
            else
            {
                constexpr const char *const riscv_assembly_GlobalConstDecl = ".section    .rodata\n"
                                                                             "    .align 2\n"
                                                                             "    .globl %s\n"
                                                                             "    .type %s, @object\n%s:\n"
                                                                             "    .word %s\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_GlobalConstDecl,
                                 variable_name, variable_name,
                                 variable_name, p_variable.getConstantPtr()->getConstantValueCString());
            }
        }
        else
        { // Local Declaration
            m_current_offset -= 4;
            symbol_entry->setOffset(m_current_offset);
            if (symbol_entry->getKind() == SymbolEntry::KindEnum::kParameterKind)
            {
                constexpr const char *const riscv_assembly_local_var_a = "    sw a%d, %d(s0)\n";
                constexpr const char *const riscv_assembly_local_var_t = "    sw t%d, %d(s0)\n";

                if (m_parameter_count < 8)
                {

                    dumpInstructions(m_output_file.get(), riscv_assembly_local_var_a, m_parameter_count, m_current_offset);
                }
                else
                {
                    dumpInstructions(m_output_file.get(), riscv_assembly_local_var_t, m_parameter_count % 8 + 2, m_current_offset);
                }
                m_parameter_count++;
            }
            else if (p_variable.getConstantPtr() != nullptr)
            {
                constexpr const char *const riscv_assembly_PushLocalAddress = "    addi t0, s0, %d\n"
                                                                              "    addi sp, sp, -4\n"
                                                                              "    sw t0, 0(sp)\n";
                constexpr const char *const riscv_assembly_PerformAssignment = "    lw t0, 0(sp)\n"
                                                                               "    addi sp, sp, 4\n"
                                                                               "    lw t1, 0(sp)\n"
                                                                               "    addi sp, sp, 4\n"
                                                                               "    sw t0, 0(t1)\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_PushLocalAddress, symbol_entry->getOffset());
                p_variable.visitChildNodes(*this);
                dumpInstructions(m_output_file.get(), riscv_assembly_PerformAssignment);
            }
        }
    }
    else
    {

        if (symbol_entry->getLevel() == 0)
        { // Global Reference
            constexpr const char *const riscv_assembly_PushGlobalAddress = "    la t0, %s\n"
                                                                           "    addi sp, sp, -4\n"
                                                                           "    sw t0, 0(sp)\n";
            constexpr const char *const riscv_assembly_PushGlobalValue = "    la t0, %s\n"
                                                                         "    lw t0, 0(t0)\n"
                                                                         "    addi sp, sp, -4\n"
                                                                         "    sw t0, 0(sp)\n";
            if (m_assign_left)
            {
                dumpInstructions(m_output_file.get(), riscv_assembly_PushGlobalAddress, variable_name);
            }
            else
            {
                dumpInstructions(m_output_file.get(), riscv_assembly_PushGlobalValue, variable_name);
            }
        }
        else
        { // Local Reference
            constexpr const char *const riscv_assembly_PushLocalAddress = "    addi t0, s0, %d\n"
                                                                          "    addi sp, sp, -4\n"
                                                                          "    sw t0, 0(sp)\n";
            constexpr const char *const riscv_assembly_PushLocalValue = "    lw t0, %d(s0)\n"
                                                                        "    addi sp, sp, -4\n"
                                                                        "    sw t0, 0(sp)\n";
            if (m_assign_left)
            {
                dumpInstructions(m_output_file.get(), riscv_assembly_PushLocalAddress, symbol_entry->getOffset());
            }
            else
            {
                dumpInstructions(m_output_file.get(), riscv_assembly_PushLocalValue, symbol_entry->getOffset());
            }
        }
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value)
{
    constexpr const char *const riscv_assembly_constant = "    li t0, %s\n"
                                                          "    addi sp, sp, -4\n"
                                                          "    sw t0, 0(sp)\n";
    const char *value = p_constant_value.getConstantValueCString();
    const char *output_value = value;
    if (p_constant_value.getTypePtr()->isBool())
        output_value = p_constant_value.getConstantPtr()->boolean() ? "1" : "0";
    dumpInstructions(m_output_file.get(), riscv_assembly_constant, output_value);
}

void CodeGenerator::visit(FunctionNode &p_function)
{
    // Reconstruct the scope for looking up the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_function)));
    m_parameter_count = 0;
    const char *function_name = p_function.getNameCString();
    constexpr const char *const riscv_assembly_function_start = "\n.section    .text\n"
                                                                "   .align 2\n"
                                                                "   .globl %s\n"
                                                                "   .type %s, @function\n\n%s:\n";
    constexpr const char *const riscv_assembly_function_end = "    .size %s, .-%s\n";

    constexpr const char *const riscv_assembly_prologue = "    addi sp, sp, -128\n"
                                                          "    sw ra, 124(sp)\n"
                                                          "    sw s0, 120(sp)\n"
                                                          "    addi s0, sp, 128\n\n";
    constexpr const char *const riscv_assembly_epilogue = "    lw ra, 124(sp)\n"
                                                          "    lw s0, 120(sp)\n"
                                                          "    addi sp, sp, 128\n"
                                                          "    jr ra\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_function_start, function_name, function_name, function_name);
    m_current_offset = -8;
    dumpInstructions(m_output_file.get(), riscv_assembly_prologue);
    p_function.visitParamChildNodes(*this);
    p_function.visitBodyChildNodes(*this);
    dumpInstructions(m_output_file.get(), riscv_assembly_epilogue);
    dumpInstructions(m_output_file.get(), riscv_assembly_function_end, function_name, function_name);

    // Remove the entries in the hash table
    m_symbol_manager.popScope();
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement)
{

    // Reconstruct the scope for looking up the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_compound_statement)));
    p_compound_statement.visitChildNodes(*this);
    m_symbol_manager.popScope();
}

void CodeGenerator::visit(PrintNode &p_print)
{
    constexpr const char *const riscv_assembly_print = "    lw a0, 0(sp)\n"
                                                       "    addi sp, sp, 4\n"
                                                       "    jal ra, printInt\n";

    p_print.visitChildNodes(*this);

    dumpInstructions(m_output_file.get(), riscv_assembly_print);
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op)
{

    p_bin_op.visitChildNodes(*this);
    m_is_binary_condition = true;
    constexpr const char *const riscv_assembly_pop_operands = "    lw t0, 0(sp)\n"
                                                              "    addi sp, sp, 4\n"
                                                              "    lw t1, 0(sp)\n"
                                                              "    addi sp, sp, 4\n";
    constexpr const char *const kPushResult = "    addi sp, sp, -4\n"
                                              "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_pop_operands);
    Operator op = p_bin_op.getOp();
    switch (op)
    {
    case Operator::kPlusOp:
        dumpInstructions(m_output_file.get(), "    add t0, t1, t0\n");
        break;
    case Operator::kMinusOp:
        dumpInstructions(m_output_file.get(), "    sub t0, t1, t0\n");
        break;
    case Operator::kMultiplyOp:
        dumpInstructions(m_output_file.get(), "    mul t0, t1, t0\n");
        break;
    case Operator::kDivideOp:
        dumpInstructions(m_output_file.get(), "    div t0, t1, t0\n");
        break;
    case Operator::kModOp:
        dumpInstructions(m_output_file.get(), "    rem t0, t1, t0\n");
        break;
    case Operator::kAndOp:
        dumpInstructions(m_output_file.get(), "    and t0, t1, t0\n");
        break;
    case Operator::kOrOp:
        dumpInstructions(m_output_file.get(), "    or t0, t1, t0\n");
        break;

    case Operator::kEqualOp:
        if (!m_assign_bool)
        {
            // if (a == b) false 時跳過 then
            dumpInstructions(m_output_file.get(),
                             "    bne t1, t0, L%d\n",
                             jump_label);
        }
        else
        {
            // result := (a == b) ? 1 : 0
            dumpInstructions(m_output_file.get(),
                             "    sub  t0, t1, t0\n"); // t0 = t1 - t0
            dumpInstructions(m_output_file.get(),
                             "    seqz t0, t0\n"); // t0 = (t0==0)?1:0
        }
        break;

    case Operator::kNotEqualOp:
        if (!m_assign_bool)
        {
            // if (a != b) false 時跳過 then
            dumpInstructions(m_output_file.get(),
                             "    beq  t1, t0, L%d\n",
                             jump_label);
        }
        else
        {
            // result := (a != b) ? 1 : 0
            dumpInstructions(m_output_file.get(),
                             "    sub  t0, t1, t0\n"); // t0 = t1 - t0
            dumpInstructions(m_output_file.get(),
                             "    snez t0, t0\n"); // t0 = (t0!=0)?1:0
        }
        break;

    case Operator::kLessOp:
        if (!m_assign_bool)
        {
            // if (a < b) false 時跳過 then
            dumpInstructions(m_output_file.get(),
                             "    bge  t1, t0, L%d\n",
                             jump_label);
        }
        else
        {
            // result := (a < b) ? 1 : 0
            dumpInstructions(m_output_file.get(),
                             "    slt  t0, t1, t0\n"); // t0 = (t1<t0)?1:0
        }
        break;

    case Operator::kGreaterOp:
        if (!m_assign_bool)
        {
            // if (a > b) false 時跳過 then
            dumpInstructions(m_output_file.get(),
                             "    ble  t1, t0, L%d\n",
                             jump_label);
        }
        else
        {
            // result := (a > b) ? 1 : 0
            dumpInstructions(m_output_file.get(),
                             "    slt  t0, t0, t1\n"); // t0 = (t0<t1)?1:0
        }
        break;

    case Operator::kLessOrEqualOp:
        if (!m_assign_bool)
        {
            // if (a <= b) false 時跳過 then
            dumpInstructions(m_output_file.get(),
                             "    bgt  t1, t0, L%d\n",
                             jump_label);
        }
        else
        {
            // result := (a <= b) ? 1 : 0
            dumpInstructions(m_output_file.get(),
                             "    slt  t0, t0, t1\n"); // t0 = (b<a)?1:0
            dumpInstructions(m_output_file.get(),
                             "    xori t0, t0, 1\n"); // t0 = !t0
        }
        break;

    case Operator::kGreaterOrEqualOp:
        if (!m_assign_bool)
        {
            // if (a >= b) false 時跳過 then
            dumpInstructions(m_output_file.get(),
                             "    blt  t1, t0, L%d\n",
                             jump_label);
        }
        else
        {
            // result := (a >= b) ? 1 : 0
            dumpInstructions(m_output_file.get(),
                             "    slt  t0, t1, t0\n"); // t0 = (a<b)?1:0
            dumpInstructions(m_output_file.get(),
                             "    xori t0, t0, 1\n"); // t0 = !t0
        }
        break;

    default:
        break;
    }
    if (op == Operator::kPlusOp || op == Operator::kMinusOp ||
        op == Operator::kMultiplyOp || op == Operator::kDivideOp ||
        op == Operator::kModOp || op == Operator::kAndOp ||
        op == Operator::kOrOp || m_assign_bool)
    {
        dumpInstructions(m_output_file.get(), kPushResult);
    }
}
void CodeGenerator::visit(UnaryOperatorNode &p_un_op)
{
    p_un_op.visitChildNodes(*this);
    constexpr const char *const riscv_assembly_unop_s = "    lw t0, 0(sp)\n"
                                                        "    addi sp, sp, 4\n";
    constexpr const char *const riscv_assembly_unop_e = "    addi sp, sp, -4\n"
                                                        "    sw t0, 0(sp)\n";

    dumpInstructions(m_output_file.get(), riscv_assembly_unop_s);
    switch (p_un_op.getOp())
    {
    case Operator::kNegOp:
    {
        constexpr const char *const riscv_assembly_unop = "    neg t0, t0\n";
        dumpInstructions(m_output_file.get(), riscv_assembly_unop);
        dumpInstructions(m_output_file.get(), riscv_assembly_unop_e);
        break;
    }
    case Operator::kNotOp:
    {
        constexpr const char *const riscv_assembly_unop = "    li t1, -1\n"
                                                          "    add t0, t0, t1\n";
        dumpInstructions(m_output_file.get(), riscv_assembly_unop);
        dumpInstructions(m_output_file.get(), riscv_assembly_unop_e);
        break;
    }
    default:
        break;
    }
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation)
{
    constexpr const char *const riscv_assembly_invocation_a = "    lw a%d, 0(sp)\n"
                                                              "    addi sp, sp, 4\n";
    constexpr const char *const riscv_assembly_invocation_t = "    lw t%d, 0(sp)\n"
                                                              "    addi sp, sp, 4\n";
    constexpr const char *const riscv_assembly_invocation_epilogue = "    jal ra, %s\n"
                                                                     "    mv t0, a0\n"
                                                                     "    addi sp, sp, -4\n"
                                                                     "    sw t0, 0(sp)\n";

    m_is_in_function_invocation = true;
    p_func_invocation.visitChildNodes(*this);
    m_is_in_function_invocation = false;
    for (int i = int(p_func_invocation.getArguments().size()) - 1; i >= 0; i--)
    {
        if (i < 8)
        {
            dumpInstructions(m_output_file.get(), riscv_assembly_invocation_a, i);
        }
        else
            dumpInstructions(m_output_file.get(), riscv_assembly_invocation_t, i % 8 + 2);
    }
    dumpInstructions(m_output_file.get(), riscv_assembly_invocation_epilogue, p_func_invocation.getNameCString());
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref)
{
    SymbolEntry *symbol_entry = const_cast<SymbolEntry *>(m_symbol_manager.lookup(p_variable_ref.getName()));
    if (!symbol_entry)
        return;
    const char *variable_name = symbol_entry->getNameCString();
    constexpr const char *const riscv_assembly_PushGlobalAddress = "    la t0, %s\n"
                                                                   "    addi sp, sp, -4\n"
                                                                   "    sw t0, 0(sp)\n";
    constexpr const char *const riscv_assembly_PushGlobalValue = "    la t0, %s\n"
                                                                 "    lw t1, 0(t0)\n"
                                                                 "    mv t0, t1\n"
                                                                 "    addi sp, sp, -4\n"
                                                                 "    sw t0, 0(sp)\n";
    bool is_get_address = (m_assign_left || m_is_in_read) && !m_is_in_function_invocation;
    if (symbol_entry->getLevel() == 0) // global
    {
        if (is_get_address)
            dumpInstructions(m_output_file.get(), riscv_assembly_PushGlobalAddress, variable_name);
        else
            dumpInstructions(m_output_file.get(), riscv_assembly_PushGlobalValue, variable_name);
    }
    else // local
    {
        constexpr const char *const riscv_assembly_PushLocalAddress = "    addi t0, s0, %d\n"
                                                                      "    addi sp, sp, -4\n"
                                                                      "    sw t0, 0(sp)\n";
        constexpr const char *const riscv_assembly_PushLocalValue = "    lw t0, %d(s0)\n"
                                                                    "    addi sp, sp, -4\n"
                                                                    "    sw t0, 0(sp)\n";
        if (is_get_address)
            dumpInstructions(m_output_file.get(), riscv_assembly_PushLocalAddress, symbol_entry->getOffset());
        else
            dumpInstructions(m_output_file.get(), riscv_assembly_PushLocalValue, symbol_entry->getOffset());
    }
}

void CodeGenerator::visit(AssignmentNode &p_assignment)
{

    constexpr const char *const riscv_assembly_assignment = "    lw t0, 0(sp)\n"
                                                            "    addi sp, sp, 4\n"
                                                            "    lw t1, 0(sp)\n"
                                                            "    addi sp, sp, 4\n"
                                                            "    sw t0, 0(t1)\n";

    m_assign_left = true;
    const_cast<VariableReferenceNode &>(p_assignment.getLvalue()).accept(*this);
    m_assign_left = false;
    const_cast<ExpressionNode &>(p_assignment.getExpr()).accept(*this);
    m_is_binary_condition = false;
    m_assign_bool = false;

    dumpInstructions(m_output_file.get(), riscv_assembly_assignment);
}
void CodeGenerator::visit(ReadNode &p_read)
{
    constexpr const char *const riscv_assembly_read = "    jal ra, readInt\n"
                                                      "    lw t0, 0(sp)\n"
                                                      "    addi sp, sp, 4\n"
                                                      "    sw a0, 0(t0)\n";
    m_is_in_read = true;
    p_read.visitChildNodes(*this);
    m_is_in_read = false;

    dumpInstructions(m_output_file.get(), riscv_assembly_read);
}

void CodeGenerator::visit(IfNode &p_if)
{
    constexpr const char *const riscv_assembly_if = "L%d:\n";
    int startLabel = 0;
    int bodyLabel = 0;
    int endLabel = 0;
    if (m_label_count != 1)
        m_label_count++;
    startLabel = m_label_count++;
    if (p_if.getBody())
    {
        endLabel = m_label_count;
        jump_label = endLabel;
    }
    if (p_if.getElseBody())
    {
        bodyLabel = m_label_count;
        jump_label = bodyLabel;
        m_label_count++;
        endLabel = m_label_count;
    }
    p_if.visitCondition(*this);
    if (m_is_binary_condition == false)
    {
        dumpInstructions(m_output_file.get(),
                         "    lw t0, 0(sp)\n"
                         "    addi sp, sp, 4\n");
        dumpInstructions(m_output_file.get(), "    beqz t0, L%d\n", bodyLabel);
    }
    m_is_binary_condition = false;
    if (p_if.getBody())
    {
        dumpInstructions(m_output_file.get(), riscv_assembly_if, startLabel);
        p_if.visitBody(*this);
        dumpInstructions(m_output_file.get(), "    j L%d\n", endLabel);
    }
    if (p_if.getElseBody())
    {
        dumpInstructions(m_output_file.get(), riscv_assembly_if, bodyLabel);
        p_if.visitElseBody(*this);
    }
    dumpInstructions(m_output_file.get(), riscv_assembly_if, endLabel);
}

void CodeGenerator::visit(WhileNode &p_while)
{
    constexpr const char *const riscv_label = "L%d:\n";
    constexpr const char *const riscv_branch_back = "    j L%d\n";

    int startLabel = m_label_count;
    if (m_label_count == 1)
    {
        dumpInstructions(m_output_file.get(), riscv_label, startLabel);
    }
    m_label_count++;

    jump_label = m_label_count + 1;
    p_while.visitCondition(*this);
    dumpInstructions(m_output_file.get(), riscv_label, m_label_count);

    if (p_while.getBody())
    {
        p_while.visitBody(*this);
    }
    dumpInstructions(m_output_file.get(), riscv_branch_back, startLabel);
    m_label_count++;
    dumpInstructions(m_output_file.get(), riscv_label, m_label_count);
}

void CodeGenerator::visit(ForNode &p_for)
{
    constexpr const char *const riscv_label = "L%d:\n";
    constexpr const char *const riscv_assembly_Push_i_Value = "    lw t0, %d(s0)\n"
                                                              "    addi sp, sp, -4\n"
                                                              "    sw t0, 0(sp)\n";
    constexpr const char *const riscv_assembly_for_condition_check = "    lw t0, 0(sp)\n"
                                                                     "    addi sp, sp, 4\n"
                                                                     "    lw t1, 0(sp)\n"
                                                                     "    addi sp, sp, 4\n"
                                                                     "    bge t1, t0, L%d\n";

    constexpr const char *const riscv_assembly_for = "    addi t0, s0, %d\n"
                                                     "    addi sp, sp, -4\n"
                                                     "    sw t0, 0(sp)\n"
                                                     "    lw t0, %d(s0)\n"
                                                     "    addi sp, sp, -4\n"
                                                     "    sw t0, 0(sp)\n"
                                                     "    li t0, 1\n"
                                                     "    addi sp, sp, -4\n"
                                                     "    sw t0, 0(sp)\n"
                                                     "    lw t0, 0(sp)\n"
                                                     "    addi sp, sp, 4\n"
                                                     "    lw t1, 0(sp)\n"
                                                     "    addi sp, sp, 4\n"
                                                     "    add t0, t1, t0\n"
                                                     "    addi sp, sp, -4\n"
                                                     "    sw t0, 0(sp)\n"
                                                     "    lw t0, 0(sp)\n"
                                                     "    addi sp, sp, 4\n"
                                                     "    lw t1, 0(sp)\n"
                                                     "    addi sp, sp, 4\n"
                                                     "    sw t0, 0(t1)\n"
                                                     "    j L%d\n";

    // Reconstruct the scope for looking up the symbol entry.

    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_for)));
    SymbolEntry *symbol_entry = const_cast<SymbolEntry *>(m_symbol_manager.lookup(p_for.getInitStmt().getLvalue().getNameCString()));
    p_for.visitLoopDeclaration(*this);
    if (m_label_count != 1)
        m_label_count++;
    int startLabel = m_label_count++; // startLabel = 6，m_if_label → 7
    int bodyLabel = m_label_count++;  // bodyLabel  = 7，m_if_label → 8
    int endLabel = m_label_count++;   // endLabel   = 8，m_if_label → 9

    dumpInstructions(m_output_file.get(), riscv_label, startLabel);
    dumpInstructions(m_output_file.get(), riscv_assembly_Push_i_Value, symbol_entry->getOffset());
    p_for.visitLoopCondition(*this);
    dumpInstructions(m_output_file.get(), riscv_assembly_for_condition_check, endLabel);
    dumpInstructions(m_output_file.get(), riscv_label, bodyLabel);
    p_for.visitLoopBody(*this);

    dumpInstructions(m_output_file.get(), riscv_assembly_for, symbol_entry->getOffset(), symbol_entry->getOffset(), startLabel);
    dumpInstructions(m_output_file.get(), riscv_label, endLabel);

    // Remove the entries in the hash table
    m_symbol_manager.popScope();
}

void CodeGenerator::visit(ReturnNode &p_return)
{
    p_return.visitChildNodes(*this);

    constexpr const char *const riscv_assembly_return = "    lw a0, 0(sp)\n"
                                                        "    addi sp, sp, 4\n"
                                                        "    mv a0, t0\n"
                                                        "    lw ra, 124(sp)\n"
                                                        "    lw s0, 120(sp)\n"
                                                        "    addi sp, sp, 128\n"
                                                        "    jr ra\n";

    dumpInstructions(m_output_file.get(), riscv_assembly_return);
}
