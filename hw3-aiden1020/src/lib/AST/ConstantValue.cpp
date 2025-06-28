#include "AST/ConstantValue.hpp"
#include <string>
#include <iostream>
// TODO

ConstantValueNode::ConstantValueNode(uint32_t line, uint32_t col, int val)
    : ExpressionNode(line, col), type(Type::Integer), s_val(nullptr)
{
  value.i_val = val;
}

ConstantValueNode::ConstantValueNode(uint32_t line, uint32_t col, double val)
    : ExpressionNode(line, col), type(Type::Real), s_val(nullptr)
{
  value.f_val = val;
}

ConstantValueNode::ConstantValueNode(uint32_t line, uint32_t col, bool val)
    : ExpressionNode(line, col), type(Type::Boolean), s_val(nullptr)
{
  value.b_val = val;
}

ConstantValueNode::ConstantValueNode(uint32_t line, uint32_t col, const std::string &val)
    : ExpressionNode(line, col), type(Type::String)
{
  s_val = new std::string(val);
}

ConstantValueNode::~ConstantValueNode()
{
  if (type == Type::String && s_val)
  {
    delete s_val;
    s_val = nullptr;
  }
}

const char *ConstantValueNode::getTypeCString() const
{
  switch (type)
  {
  case Type::Integer:
    return "integer";
  case Type::Real:
    return "real";
  case Type::Boolean:
    return "boolean";
  case Type::String:
    return "string";
  }
  return "unknown";
}

int ConstantValueNode::getIntValue() const
{
  return value.i_val;
}
double ConstantValueNode::getRealValue() const
{
  return value.f_val;
}
bool ConstantValueNode::getBoolValue() const
{
  return value.b_val;
}
const char *ConstantValueNode::getStringValue() const
{
  return s_val->c_str();
}

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {}
void ConstantValueNode::accept(AstNodeVisitor &p_visitor)
{
  p_visitor.visit(*this);
}