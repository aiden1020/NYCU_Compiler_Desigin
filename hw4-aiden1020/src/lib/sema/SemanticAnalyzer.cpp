#include "sema/Error.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"
#include <iostream>

void SemanticAnalyzer::visit(ProgramNode &p_program)
{

    symbol_manager.pushScope();
    current_symbol_kind = SymbolEntry::SymbolKind::kProgram;
    SymbolTable *current_table = symbol_manager.getCurrentTable();
    auto type_ptr = std::make_shared<PType>(PType::PrimitiveTypeEnum::kVoidType);
    current_table->addSymbol(p_program.getLocation(), p_program.getNameCString(),
                             SymbolEntry::SymbolKind::kProgram, 0, type_ptr, nullptr);
    p_program.visitChildNodes(*this);
    const auto &body_stmts = p_program.getBody()->getStatements();
    for (auto &stmt : body_stmts)
    {
        if (auto return_stmt = dynamic_cast<ReturnNode *>(stmt.get()))
        {
            return_stmt->visitChildNodes(*this);
            auto return_type = return_stmt->getReturnValue()
                                   ? return_stmt->getReturnValue()->getReturnType()
                                   : nullptr;
            if (return_type && type_ptr->getPrimitiveType() == PType::PrimitiveTypeEnum::kVoidType)
            {
                ReturnFromVoidError err(return_stmt->getLocation());
                m_error_printer.print(err);
                has_error = true;
            }
        }
    }
    symbol_manager.popScope(dump_table);
}

void SemanticAnalyzer::visit(DeclNode &p_decl)
{
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable)
{

    p_variable.visitChildNodes(*this);
    SymbolTable *current_table = symbol_manager.getCurrentTable();
    auto type_ptr = p_variable.getTypeSharedPtr();
    int level = current_table->getLevel();

    auto redecl_entry = current_table->lookupLocal(p_variable.getNameCString());
    if (redecl_entry != nullptr)
    {
        SymbolRedeclarationError err(p_variable.getLocation(), p_variable.getNameCString());
        m_error_printer.print(err);
        has_error = true;
        return;
    }

    SymbolEntry *parent_entry = current_table->lookupDecl(p_variable.getNameCString());
    if (parent_entry &&
        (parent_entry->getKind() == SymbolEntry::SymbolKind::kLoopVar))
    {
        SymbolRedeclarationError err(p_variable.getLocation(), p_variable.getNameCString());
        m_error_printer.print(err);
        has_error = true;
        return;
    }

    SymbolEntry *entry_ptr = nullptr;
    switch (current_symbol_kind)
    {
    case SymbolEntry::SymbolKind::kConstant:
        entry_ptr = &current_table->addSymbol(
            p_variable.getLocation(), p_variable.getNameCString(),
            SymbolEntry::SymbolKind::kConstant, level, type_ptr, p_variable.getConstantPtr());
        break;
    case SymbolEntry::SymbolKind::kFunction:
        entry_ptr = &current_table->addSymbol(
            p_variable.getLocation(), p_variable.getNameCString(),
            SymbolEntry::SymbolKind::kParameter, level, type_ptr, nullptr);
        break;
    case SymbolEntry::SymbolKind::kLoopVar:
        entry_ptr = &current_table->addSymbol(
            p_variable.getLocation(), p_variable.getNameCString(),
            SymbolEntry::SymbolKind::kLoopVar, level, type_ptr, nullptr);
        break;
    default:
        entry_ptr = &current_table->addSymbol(
            p_variable.getLocation(), p_variable.getNameCString(),
            SymbolEntry::SymbolKind::kVariable, level, type_ptr, nullptr);
        break;
    }

    if (!p_variable.getTypeSharedPtr()->checkDimensionsValid() && entry_ptr)
    {
        entry_ptr->setInvalid(true);
        NonPositiveArrayDimensionError err(p_variable.getLocation(), p_variable.getNameCString());
        m_error_printer.print(err);
        has_error = true;
    }
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value)
{

    current_symbol_kind = SymbolEntry::SymbolKind::kConstant;
    p_constant_value.setReturnType(p_constant_value.getConstant()->getTypeSharedPtr());
}

void SemanticAnalyzer::visit(FunctionNode &p_function)
{

    SymbolTable *current_table = symbol_manager.getCurrentTable();
    current_symbol_kind = SymbolEntry::SymbolKind::kFunction;
    auto type_ptr = p_function.getRetTypeSharedPtr();
    int level = current_table->getLevel();
    std::vector<std::string> param_type_strs;
    for (auto &param : p_function.getParameters())
    {
        for (auto &var : param->getVariables())
        {
            param_type_strs.push_back(var->getTypeSharedPtr()->getPTypeCString());
        }
    }
    std::string attribute;
    if (!param_type_strs.empty())
    {
        attribute = param_type_strs[0];
        for (size_t i = 1; i < param_type_strs.size(); ++i)
        {
            attribute += ", " + param_type_strs[i];
        }
    }
    else
    {
        attribute = "";
    }

    // check redeclaration
    auto redecl_entry = current_table->lookupDecl(p_function.getNameCString());
    if (!redecl_entry || (redecl_entry->getLevel() == 0 && level != 0))
    {
        current_table->addSymbol(p_function.getLocation(), p_function.getNameCString(),
                                 SymbolEntry::SymbolKind::kFunction, level,
                                 type_ptr, attribute.c_str());
    }
    else
    {
        // redeclaration error
        SymbolRedeclarationError err(p_function.getLocation(), p_function.getNameCString());
        m_error_printer.print(err);
        has_error = true;
    }

    symbol_manager.pushScope(current_table);
    p_function.visitChildNodes(*this);
    auto *body = p_function.getBody();
    if (!body)
    {
        symbol_manager.popScope(dump_table);
        return; // 或其他錯誤處理
    }
    const auto &body_stmts = p_function.getBody()->getStatements();
    for (const auto &stmt : body_stmts)
    {
        if (auto return_stmt = dynamic_cast<ReturnNode *>(stmt.get()))
        {
            return_stmt->visitChildNodes(*this);

            auto return_type = return_stmt->getReturnValue()
                                   ? return_stmt->getReturnValue()->getReturnType()
                                   : nullptr;
            if (return_type && !is_argument_type_compatible(return_type, type_ptr.get()))
            {
                IncompatibleReturnTypeError err(
                    return_stmt->getReturnValue()->getLocation(), type_ptr.get(), return_type);
                m_error_printer.print(err);
                has_error = true;
            }
        }
    }
    symbol_manager.popScope(dump_table);
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement)
{
    auto current_table = symbol_manager.getCurrentTable();

    bool new_scope_flag =
        !(current_symbol_kind == SymbolEntry::SymbolKind::kFunction);

    if (new_scope_flag)
    {
        symbol_manager.pushScope(current_table);
    }

    for (auto &decl : p_compound_statement.getDeclarations())
    {
        current_symbol_kind = decl->isConstantDecl()
                                  ? SymbolEntry::SymbolKind::kConstant
                                  : SymbolEntry::SymbolKind::kVariable;
        decl->accept(*this);
    }

    for (auto &stmt : p_compound_statement.getStatements())
    {
        current_symbol_kind = SymbolEntry::SymbolKind::kVariable;
        stmt->accept(*this);
    }

    if (new_scope_flag)
    {
        symbol_manager.popScope(dump_table);
    }
    else
    {
        current_symbol_kind = SymbolEntry::SymbolKind::kVariable;
    }
}

void SemanticAnalyzer::visit(PrintNode &p_print)
{

    p_print.visitChildNodes(*this);
    auto expr = p_print.getTarget();
    auto expr_type = expr->getReturnType();
    if (!expr_type)
    {
        return;
    }
    if (!expr_type ||
        expr_type->getPrimitiveType() == PType::PrimitiveTypeEnum::kVoidType ||
        !expr_type->getDimensions().empty())
    {
        PrintOutNonScalarTypeError err(expr->getLocation());
        m_error_printer.print(err);
        has_error = true;
        return;
    }
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op)
{
    p_bin_op.visitChildNodes(*this);

    auto lhs_expr = p_bin_op.getLeftOperand();
    auto rhs_expr = p_bin_op.getRightOperand();
    const PType *lhs_type = lhs_expr ? lhs_expr->getReturnType() : nullptr;
    const PType *rhs_type = rhs_expr ? rhs_expr->getReturnType() : nullptr;

    if (!lhs_type || !rhs_type)
        return;

    auto op = p_bin_op.getOp();

    auto is_arithmetic_type = [](const PType *t)
    {
        return t && t->getDimensions().empty() &&
               (t->getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType ||
                t->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType);
    };
    auto is_string_type = [](const PType *t)
    {
        return t && t->getDimensions().empty() &&
               t->getPrimitiveType() == PType::PrimitiveTypeEnum::kStringType;
    };
    auto is_boolean_type = [](const PType *t)
    {
        return t && t->getDimensions().empty() &&
               t->getPrimitiveType() == PType::PrimitiveTypeEnum::kBoolType;
    };

    bool type_error = false;
    switch (op)
    {
    case Operator::kPlusOp:
    {
        bool both_string = is_string_type(lhs_type) && is_string_type(rhs_type);
        bool both_number = is_arithmetic_type(lhs_type) && is_arithmetic_type(rhs_type);
        if (!both_string && !both_number)
            type_error = true;
        else
        {
            if (both_string)
                p_bin_op.setReturnType(PType::PrimitiveTypeEnum::kStringType);
            else if (both_number)
                p_bin_op.setReturnType(
                    (lhs_type->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType ||
                     rhs_type->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType)
                        ? PType::PrimitiveTypeEnum::kRealType
                        : PType::PrimitiveTypeEnum::kIntegerType);
        }
        break;
    }
    case Operator::kMinusOp:
    case Operator::kMultiplyOp:
    case Operator::kDivideOp:
    {
        if (!is_arithmetic_type(lhs_type) || !is_arithmetic_type(rhs_type))
            type_error = true;
        else
        {
            p_bin_op.setReturnType(
                (lhs_type->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType ||
                 rhs_type->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType)
                    ? PType::PrimitiveTypeEnum::kRealType
                    : PType::PrimitiveTypeEnum::kIntegerType);
        }
        break;
    }
    case Operator::kModOp:
    {
        auto is_int = [](const PType *t)
        {
            return t && t->getDimensions().empty() &&
                   t->getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType;
        };
        if (!is_int(lhs_type) || !is_int(rhs_type))
            type_error = true;
        else
            p_bin_op.setReturnType(PType::PrimitiveTypeEnum::kIntegerType);
        break;
    }
    case Operator::kAndOp:
    case Operator::kOrOp:
    {
        if (!is_boolean_type(lhs_type) || !is_boolean_type(rhs_type))
            type_error = true;
        else
            p_bin_op.setReturnType(PType::PrimitiveTypeEnum::kBoolType);
        break;
    }
    case Operator::kLessOp:
    case Operator::kGreaterOp:
    case Operator::kEqualOp:
    case Operator::kNotEqualOp:
    case Operator::kLessOrEqualOp:
    case Operator::kGreaterOrEqualOp:
    {
        if (is_arithmetic_type(lhs_type) && is_arithmetic_type(rhs_type))
        {
            p_bin_op.setReturnType(PType::PrimitiveTypeEnum::kBoolType);
        }
        else
        {
            type_error = true;
        }
        break;
    }
    case Operator::kNegOp:
    case Operator::kNotOp:
        break;
    }

    if (type_error)
    {
        InvalidBinaryOperandError err(
            p_bin_op.getLocation(), p_bin_op.getOp(), lhs_type, rhs_type);
        m_error_printer.print(err);
        has_error = true;
    }
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op)
{
    p_un_op.visitChildNodes(*this);

    auto expr = p_un_op.getOperand();
    const PType *type = expr ? expr->getReturnType() : nullptr;
    if (!type)
    {
        return;
    }

    if (p_un_op.getOp() == Operator::kNotOp)
    {
        if (type->getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType ||
            !type->getDimensions().empty())
        {
            InvalidUnaryOperandError err(
                p_un_op.getLocation(), p_un_op.getOp(), type);
            m_error_printer.print(err);
            has_error = true;
            return;
        }
        p_un_op.setReturnType(PType::PrimitiveTypeEnum::kBoolType);
    }
    else if (p_un_op.getOp() == Operator::kNegOp)
    {
        if ((type->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType &&
             type->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType) ||
            !type->getDimensions().empty())
        {
            InvalidUnaryOperandError err(
                p_un_op.getLocation(), p_un_op.getOp(), type);
            m_error_printer.print(err);
            has_error = true;
            return;
        }
        p_un_op.setReturnType(type->getPrimitiveType());
    }
    else
    {
        InvalidUnaryOperandError err(
            p_un_op.getLocation(), p_un_op.getOp(), type);
        m_error_printer.print(err);
        has_error = true;
        return;
    }
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation)
{
    p_func_invocation.visitChildNodes(*this);

    auto current_table = symbol_manager.getCurrentTable();
    auto entry = current_table->lookupDecl(p_func_invocation.getNameCString());
    if (!entry)
    {
        UndeclaredSymbolError err(p_func_invocation.getLocation(), p_func_invocation.getNameCString());
        m_error_printer.print(err);
        has_error = true;
        p_func_invocation.setReturnType(nullptr);
        return;
    }

    if (entry->getKind() != SymbolEntry::SymbolKind::kFunction)
    {
        NonFunctionSymbolError err(p_func_invocation.getLocation(), p_func_invocation.getNameCString());
        m_error_printer.print(err);
        has_error = true;
        p_func_invocation.setReturnType(nullptr);
        return;
    }
    std::string param_str = entry->getParameterAttribute();
    std::vector<std::string> param_types;

    if (!param_str.empty())
    {
        std::istringstream iss(param_str);
        std::string token;
        while (std::getline(iss, token, ','))
        {
            token.erase(0, token.find_first_not_of(' '));
            token.erase(token.find_last_not_of(' ') + 1);
            if (!token.empty())
                param_types.push_back(token);
        }
    }
    if (param_types.size() != p_func_invocation.getArguments().size())
    {
        ArgumentNumberMismatchError err(p_func_invocation.getLocation(), p_func_invocation.getNameCString());
        m_error_printer.print(err);
        has_error = true;
        return;
    }
    std::vector<PType *> expected_types;

    for (size_t i = 0; i < param_types.size(); ++i)
    {
        auto arg = p_func_invocation.getArguments()[i].get();
        auto arg_type = arg->getReturnType();

        if (!arg_type)
        {
            return;
        }

        PType *expected_type = stringToPType(param_types[i]);
        expected_types.push_back(expected_type);

        if (!expected_type || !is_argument_type_compatible(arg_type, expected_type))
        {
            IncompatibleArgumentTypeError err(
                arg->getLocation(),
                expected_type,
                arg_type);
            m_error_printer.print(err);
            has_error = true;
            for (auto *t : expected_types)
                delete t;
            return;
        }
    }

    for (auto *t : expected_types)
        delete t;

    PTypeSharedPtr return_type = entry->getType();
    p_func_invocation.setReturnType(return_type);
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref)
{
    p_variable_ref.visitChildNodes(*this);
    auto current_table = symbol_manager.getCurrentTable();
    auto entry = current_table->lookupDecl(p_variable_ref.getNameCString());
    if (!entry)
    {
        UndeclaredSymbolError err(p_variable_ref.getLocation(), p_variable_ref.getNameCString());
        m_error_printer.print(err);
        has_error = true;
        return;
    }

    if (entry->getKind() == SymbolEntry::SymbolKind::kFunction)
    {
        NonVariableSymbolError err(p_variable_ref.getLocation(), p_variable_ref.getNameCString());
        m_error_printer.print(err);
        has_error = true;
        return;
    }
    if (entry->isInvalid())
    {
        return;
    }

    int idx_pos = 0;
    for (const auto &idx_expr_ptr : p_variable_ref.getIndices())
    {
        bool is_integer_index = false;
        auto idx_type = idx_expr_ptr->getReturnType();
        if (!idx_type)
            return;
        if (auto const_val = dynamic_cast<ConstantValueNode *>(idx_expr_ptr.get()))
        {
            auto type_ptr = const_val->getTypeSharedPtr();

            if (type_ptr->getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType)
            {
                is_integer_index = true;
            }
        }

        if (!is_integer_index)
        {
            NonIntegerArrayIndexError err(idx_expr_ptr->getLocation());
            m_error_printer.print(err);
            has_error = true;
            return;
        }
        ++idx_pos;
    }
    int declared_dim = entry->getType()->getDimensions().size();
    int used_dim = p_variable_ref.getIndices().size();
    if (used_dim > declared_dim)
    {
        OverArraySubscriptError err(p_variable_ref.getLocation(), p_variable_ref.getNameCString());
        m_error_printer.print(err);
        has_error = true;
        return;
    }

    PTypeSharedPtr base_type = entry->getType();
    if (used_dim == declared_dim)
    {
        p_variable_ref.setReturnType(base_type->getPrimitiveType());
    }
    else
    {
        std::vector<uint64_t> remain_dims(
            base_type->getDimensions().begin() + used_dim,
            base_type->getDimensions().end());
        auto new_type = std::make_shared<PType>(base_type->getPrimitiveType());
        new_type->setDimensions(remain_dims);
        p_variable_ref.setReturnType(new_type);
    }
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment)
{
    p_assignment.visitChildNodes(*this);
    auto left_value = p_assignment.getLeftValue();
    auto right_value = p_assignment.getRightValue();
    auto left_type = left_value->getReturnType();
    auto right_type = right_value->getReturnType();

    if (auto left_value_ref = dynamic_cast<VariableReferenceNode *>(left_value))
    {
        auto entry = symbol_manager.getCurrentTable()->lookupDecl(left_value_ref->getNameCString());
        if (entry)
        {
            if (entry->getKind() == SymbolEntry::SymbolKind::kConstant)
            {
                AssignToConstantError err(left_value_ref->getLocation(), left_value_ref->getNameCString());
                m_error_printer.print(err);
                has_error = true;
                return;
            }
            if (entry->getKind() == SymbolEntry::SymbolKind::kLoopVar && !in_loop_var_init_scope())
            {
                AssignToLoopVarError err(left_value_ref->getLocation());
                m_error_printer.print(err);
                has_error = true;
                return;
            }
        }
    }
    if (!left_type || !right_type)
        return;
    if (!left_type->getDimensions().empty())
    {
        AssignWithArrayTypeError err(left_value->getLocation());
        m_error_printer.print(err);
        has_error = true;
        return;
    }
    if (!right_type->getDimensions().empty())
    {
        AssignWithArrayTypeError err(right_value->getLocation());
        m_error_printer.print(err);
        has_error = true;
        return;
    }
    if (!is_argument_type_compatible(right_type, left_type))
    {
        IncompatibleAssignmentError err(
            p_assignment.getLocation(),
            left_type,
            right_type);
        m_error_printer.print(err);
        has_error = true;
        return;
    }
}

void SemanticAnalyzer::visit(ReadNode &p_read)
{
    p_read.visitChildNodes(*this);
    auto expr = p_read.getTarget();
    auto expr_type = expr->getReturnType();
    if (!expr_type)
    {
        return;
    }
    if (!expr_type->getDimensions().empty())
    {
        ReadToNonScalarTypeError err(expr->getLocation());
        m_error_printer.print(err);
        has_error = true;
        return;
    }
    SymbolTable *current_table = symbol_manager.getCurrentTable();
    auto entry = current_table->lookupDecl(p_read.getTargetNameCString());
    if (entry)
    {
        if (entry->getKind() == SymbolEntry::SymbolKind::kConstant || entry->getKind() == SymbolEntry::SymbolKind::kLoopVar)
        {
            ReadToConstantOrLoopVarError err(expr->getLocation());
            m_error_printer.print(err);
            has_error = true;
            return;
        }
    }
}

void SemanticAnalyzer::visit(IfNode &p_if)
{
    p_if.visitChildNodes(*this);
    auto cond_expr = p_if.getCondition();
    if (auto cond_type = cond_expr->getReturnType())
    {
        if (cond_type->getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType)
        {
            NonBooleanConditionError err(cond_expr->getLocation());
            m_error_printer.print(err);
            has_error = true;
        }
    }
}

void SemanticAnalyzer::visit(WhileNode &p_while)
{

    p_while.visitChildNodes(*this);
    current_symbol_kind = SymbolEntry::SymbolKind::kLoopVar;
}

void SemanticAnalyzer::visit(ForNode &p_for)
{

    auto current_table = symbol_manager.getCurrentTable();
    current_symbol_kind = SymbolEntry::SymbolKind::kLoopVar;
    symbol_manager.pushScope(current_table);
    if (auto *decl = p_for.getLoopVarDecl())
    {
        decl->accept(*this);
    }

    loop_var_init_stack.push_back(true);
    if (auto *init = p_for.getInitAssignment())
    {
        init->accept(*this);
    }
    loop_var_init_stack.pop_back();
    auto lower_bound_value = std::atoi(dynamic_cast<ConstantValueNode *>(p_for.getInitAssignment()->getRightValue())->getConstantValueCString());
    if (auto *end_cond = p_for.getEndCondition())
    {
        end_cond->accept(*this);
    }
    auto upper_bound_value = std::atoi(dynamic_cast<ConstantValueNode *>(p_for.getEndCondition())->getConstantValueCString());
    if (upper_bound_value < lower_bound_value)
    {
        NonIncrementalLoopVariableError err(p_for.getLocation());
        m_error_printer.print(err);
        has_error = true;
    }
    if (auto *body = p_for.getBody())
    {
        body->accept(*this);
    }
    symbol_manager.popScope(dump_table);
}

void SemanticAnalyzer::visit(ReturnNode &p_return)
{
}
