#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(uint32_t line, uint32_t col,
                           std::string p_name,
                           std::string p_type,
                           ConstantValueNode *p_const)
    : AstNode(line, col),
      name(p_name),
      type(p_type),
      constant(p_const)
{
}

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
  // TODO
  if (constant)
  {
    constant->accept(p_visitor);
  }
}
