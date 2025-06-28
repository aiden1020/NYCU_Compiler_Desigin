#include "AST/UnaryOperator.hpp"
#include <iostream>
// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
                                     std::string p_operator,
                                     ExpressionNode *p_expression)
    : ExpressionNode{line, col},
      op(p_operator),
      expression(p_expression)
{
}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    // TODO
    if (expression)
    {
        expression->accept(p_visitor);
    }
}
