#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <memory>

class BinaryOperatorNode : public ExpressionNode
{
public:
  BinaryOperatorNode(const uint32_t line, const uint32_t col,
                     std::string p_operator,
                     ExpressionNode *p_left,
                     ExpressionNode *p_right
                     /* TODO: operator, expressions */);
  ~BinaryOperatorNode() = default;
  const char *getOperatorCString() const { return op.c_str(); }
  void print() override;
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: operator, expressions
  std::string op;
  ExpressionNode *left;
  ExpressionNode *right;
};

#endif
