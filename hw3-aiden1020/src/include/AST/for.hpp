#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/assignment.hpp"
#include "visitor/AstNodeVisitor.hpp"
class ForNode : public AstNode
{
public:
  ForNode(const uint32_t line, const uint32_t col,
          DeclNode *p_declaration,
          AssignmentNode *p_assignment,
          ExpressionNode *p_expression,
          CompoundStatementNode *p_compound_statement
          /* TODO: declaration, assignment, expression,
           *       compound statement */
  );
  ~ForNode() = default;

  void print() override;
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: declaration, assignment, expression, compound statement
  DeclNode *declaration;
  AssignmentNode *assignment;
  ExpressionNode *expression;
  CompoundStatementNode *compound_statement;
};

#endif
