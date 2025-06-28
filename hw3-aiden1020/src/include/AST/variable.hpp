#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <string>
#include <iostream>
class VariableNode : public AstNode
{
public:
  VariableNode(const uint32_t line, const uint32_t col,
               std::string p_name,
               std::string p_type,
               ConstantValueNode *const p_constant_value
               /* TODO: variable name, type, constant value */);
  ~VariableNode() = default;

  void print() override;
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;
  void setType(std::string p_type) { type = p_type; };
  void setConstantValue(ConstantValueNode *const p_constant_value)
  {
    constant = p_constant_value;
  };
  const char *getNameCString() const { return name.c_str(); }
  const char *getTypeCString() const { return type.c_str(); }

private:
  std::string name;            // variable name
  std::string type;            // variable type
  ConstantValueNode *constant; // constant value
};

#endif
