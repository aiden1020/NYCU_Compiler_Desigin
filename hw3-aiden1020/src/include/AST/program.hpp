#ifndef AST_PROGRAM_NODE_H
#define AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

#include <string>
#include <vector>

class ProgramNode final : public AstNode
{
private:
  std::string name;
  std::vector<DeclNode *> var_decls;
  std::vector<FunctionNode *> func_decls;
  CompoundStatementNode *m_body;
  // TODO: return type, declarations, functions, compound statement

public:
  ~ProgramNode() = default;
  ProgramNode(const uint32_t line, const uint32_t col,
              const std::string p_name,
              std::vector<DeclNode *> p_decls,
              std::vector<FunctionNode *> p_func_decls,
              CompoundStatementNode *const p_body
              /* TODO: return type, declarations, functions,
               *       compound statement */
  );

  // visitor pattern version: const char *getNameCString() const;
  const char *getNameCString() const { return name.c_str(); }
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;
  void print() override;
};

#endif
