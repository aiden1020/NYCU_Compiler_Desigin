#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>
#include <string>
#include <vector>

class FunctionNode : public AstNode
{
public:
  FunctionNode(const uint32_t line, const uint32_t col,
               std::string p_name,
               std::vector<DeclNode *> p_decls,
               std::string p_return_type,
               CompoundStatementNode *const p_body
               /* TODO: name, declarations, return type,
                *       compound statement (optional) */
  );

  ~FunctionNode() = default;

  void print() override;
  const char *getNameCString() const { return name.c_str(); }
  const char *getReturnTypeCString() const { return return_type.c_str(); }
  const std::vector<DeclNode *> &getDeclarations() const { return var_decls; }
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: name, declarations, return type, compound statement
  std::string name;
  std::vector<DeclNode *> var_decls;
  std::string return_type;
  CompoundStatementNode *m_body;
};

#endif
