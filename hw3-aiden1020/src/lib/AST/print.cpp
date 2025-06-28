#include "AST/print.hpp"
#include <iostream>
// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col,
                     ExpressionNode *p_expression)
    : AstNode{line, col},
      expression(p_expression)
{
}

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    // TODO
    if (expression)
    {
        expression->accept(p_visitor);
    }
}
