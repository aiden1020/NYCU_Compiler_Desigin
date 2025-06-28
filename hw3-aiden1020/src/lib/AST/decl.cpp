#include "AST/decl.hpp"
#include <iostream>
// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
                   std::vector<VariableNode *> p_identifiers,
                   std::string p_type
                   /* TODO: identifiers, type */)
    : AstNode{line, col},
      identifiers(p_identifiers),
      type(p_type),
      constant(nullptr)
{
}

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
                   std::vector<VariableNode *> p_identifiers,
                   ConstantValueNode *const p_constant_value)
    : AstNode{line, col},
      identifiers(p_identifiers),
      constant(p_constant_value)
{
}
// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
  // TODO
  for (auto &identifier : identifiers)
  {
    identifier->accept(p_visitor);
  }
}
