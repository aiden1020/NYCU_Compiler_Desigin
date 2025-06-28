#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <vector>
class VariableReferenceNode : public ExpressionNode
{
public:
  // normal reference
  VariableReferenceNode(const uint32_t line, const uint32_t col, std::string p_name

                        /* TODO: name */);
  // array reference
  VariableReferenceNode(const uint32_t line, const uint32_t col,
                        std::string p_name,
                        std::vector<ExpressionNode *> p_expressions
                        /* TODO: name, expressions */);
  ~VariableReferenceNode() = default;

  void print() override;
  const char *getNameCString() const { return name.c_str(); }
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: variable name, expressions
  std::string name;
  std::vector<ExpressionNode *> expressions;
};

#endif
