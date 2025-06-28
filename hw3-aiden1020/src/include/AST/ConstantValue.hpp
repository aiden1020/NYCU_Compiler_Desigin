#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <string>
#include <iostream>
class ConstantValueNode : public ExpressionNode
{
public:
  enum class Type
  {
    Integer,
    Real,
    Boolean,
    String
  };

  ConstantValueNode(uint32_t line, uint32_t col, int val);
  ConstantValueNode(uint32_t line, uint32_t col, double val);
  ConstantValueNode(uint32_t line, uint32_t col, bool val);
  ConstantValueNode(uint32_t line, uint32_t col, const std::string &val);

  ~ConstantValueNode();

  void print() override;
  Type getType() const { return type; }

  const char *getTypeCString() const;
  int getIntValue() const;
  double getRealValue() const;
  bool getBoolValue() const;
  const char *getStringValue() const;

  void accept(AstNodeVisitor &p_visitor) override;

private:
  Type type;

  union Value
  {
    int i_val;
    double f_val;
    bool b_val;
  };
  Value value;

  std::string *s_val;
};

#endif
