#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col,
                   VariableReferenceNode *p_variable_ref)
    : AstNode{line, col},
      variable_ref(p_variable_ref)
{
}

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {}

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    // TODO
    if (variable_ref)
    {
        variable_ref->accept(p_visitor);
    }
}
