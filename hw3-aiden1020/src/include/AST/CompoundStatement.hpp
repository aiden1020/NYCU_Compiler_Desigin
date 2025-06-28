#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <vector>
#include <string>

class CompoundStatementNode : public AstNode
{
public:
  CompoundStatementNode(const uint32_t line, const uint32_t col,
                        std::vector<DeclNode *> p_decls,
                        std::vector<AstNode *> p_stmts
                        /* TODO: declarations, statements */);
  ~CompoundStatementNode() = default;

  void print() override;
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: declarations, statements
  std::vector<DeclNode *> var_decls;
  std::vector<AstNode *> stmts;
};

#endif
