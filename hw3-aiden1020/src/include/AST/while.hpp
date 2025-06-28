#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
class WhileNode : public AstNode
{
public:
  WhileNode(const uint32_t line, const uint32_t col,
            ExpressionNode *p_expression,
            CompoundStatementNode *p_compound_statement
            /* TODO: expression, compound statement */);
  ~WhileNode() = default;

  void print() override;
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: expression, compound statement
  ExpressionNode *expression;
  CompoundStatementNode *compound_statement;
};

#endif
