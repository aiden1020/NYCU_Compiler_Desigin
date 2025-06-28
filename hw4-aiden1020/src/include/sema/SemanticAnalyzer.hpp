#ifndef SEMA_SEMANTIC_ANALYZER_H
#define SEMA_SEMANTIC_ANALYZER_H

#include "sema/ErrorPrinter.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "sema/SymbolManager.hpp"
#include <sstream>
#include <regex>
#include <vector>
class SemanticAnalyzer final : public AstNodeVisitor
{
private:
  ErrorPrinter m_error_printer{stderr};
  // TODO: something like symbol manager (manage symbol tables)
  //       context manager, return type manager
  SymbolManager symbol_manager;
  SymbolEntry::SymbolKind current_symbol_kind;
  bool dump_table = true;
  bool has_error = false;
  std::vector<bool> loop_var_init_stack;

public:
  ~SemanticAnalyzer() = default;
  SemanticAnalyzer(uint8_t dump_table) : dump_table(dump_table) {}
  bool getHasError() const { return has_error; }
  void visit(ProgramNode &p_program) override;
  void visit(DeclNode &p_decl) override;
  void visit(VariableNode &p_variable) override;
  void visit(ConstantValueNode &p_constant_value) override;
  void visit(FunctionNode &p_function) override;
  void visit(CompoundStatementNode &p_compound_statement) override;
  void visit(PrintNode &p_print) override;
  void visit(BinaryOperatorNode &p_bin_op) override;
  void visit(UnaryOperatorNode &p_un_op) override;
  void visit(FunctionInvocationNode &p_func_invocation) override;
  void visit(VariableReferenceNode &p_variable_ref) override;
  void visit(AssignmentNode &p_assignment) override;
  void visit(ReadNode &p_read) override;
  void visit(IfNode &p_if) override;
  void visit(WhileNode &p_while) override;
  void visit(ForNode &p_for) override;
  void visit(ReturnNode &p_return) override;
  bool in_loop_var_init_scope() const { return !loop_var_init_stack.empty() && loop_var_init_stack.back(); }

  PType *stringToPType(const std::string &type_str)
  {
    using PrimitiveTypeEnum = PType::PrimitiveTypeEnum;
    PrimitiveTypeEnum ptype;
    if (type_str.find("integer") == 0)
      ptype = PrimitiveTypeEnum::kIntegerType;
    else if (type_str.find("real") == 0)
      ptype = PrimitiveTypeEnum::kRealType;
    else if (type_str.find("boolean") == 0)
      ptype = PrimitiveTypeEnum::kBoolType;
    else if (type_str.find("string") == 0)
      ptype = PrimitiveTypeEnum::kStringType;
    else
      return nullptr;

    std::vector<uint64_t> dims;
    std::regex re(R"(\[(\d+)\])");
    auto it = std::sregex_iterator(type_str.begin(), type_str.end(), re);
    for (; it != std::sregex_iterator(); ++it)
      dims.push_back(std::stoul((*it)[1].str()));

    PType *t = new PType(ptype);
    t->setDimensions(dims);
    return t;
  }
  bool is_argument_type_compatible(const PType *arg_type, const PType *param_type)
  {
    if (!arg_type || !param_type)
      return false;

    if (arg_type->getDimensions() != param_type->getDimensions())
      return false;

    auto arg_prim = arg_type->getPrimitiveType();
    auto param_prim = param_type->getPrimitiveType();

    if (arg_prim == param_prim)
      return true;

    if (arg_type->getDimensions().empty() &&
        arg_prim == PType::PrimitiveTypeEnum::kIntegerType &&
        param_prim == PType::PrimitiveTypeEnum::kRealType)
      return true;

    return false;
  }
};

#endif
