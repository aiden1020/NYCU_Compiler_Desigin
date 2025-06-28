#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
class IfNode : public AstNode
{
public:
  IfNode(const uint32_t line, const uint32_t col,
         ExpressionNode *p_expression,
         CompoundStatementNode *p_compound_statement,
         CompoundStatementNode *p_else_compound_statement
         /* TODO: expression, compound statement, compound statement */);
  ~IfNode() = default;

  void print() override;
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: expression, compound statement, compound statement
  ExpressionNode *expression;
  CompoundStatementNode *compound_statement;
  CompoundStatementNode *else_compound_statement;
};

#endif
