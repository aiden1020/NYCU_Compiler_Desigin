#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
class UnaryOperatorNode : public ExpressionNode
{
public:
  UnaryOperatorNode(const uint32_t line, const uint32_t col,
                    std::string p_operator,
                    ExpressionNode *p_expression
                    /* TODO: operator, expression */);
  ~UnaryOperatorNode() = default;
  const char *getOperatorCString() const { return op.c_str(); }
  void print() override;
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: operator, expression
  std::string op;
  ExpressionNode *expression;
};

#endif
