#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ReadNode : public AstNode
{
public:
  ReadNode(const uint32_t line, const uint32_t col,
           VariableReferenceNode *p_variable_ref
           /* TODO: variable reference */);
  ~ReadNode() = default;

  void print() override;
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: variable reference
  VariableReferenceNode *variable_ref;
};

#endif
