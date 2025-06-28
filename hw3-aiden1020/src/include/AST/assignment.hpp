#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/VariableReference.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

class AssignmentNode : public AstNode
{
public:
  AssignmentNode(const uint32_t line, const uint32_t col,
                 VariableReferenceNode *p_variable_ref,
                 ExpressionNode *p_expression
                 /* TODO: variable reference, expression */);
  ~AssignmentNode() = default;

  void print() override;
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: variable reference, expression
  VariableReferenceNode *variable_ref;
  ExpressionNode *expression;
};

#endif
