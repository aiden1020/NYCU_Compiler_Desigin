#include "AST/BinaryOperator.hpp"
#include <iostream>
// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col,
                                       std::string p_operator,
                                       ExpressionNode *p_left,
                                       ExpressionNode *p_right)
    : ExpressionNode{line, col},
      op(p_operator),
      left(std::move(p_left)),
      right(std::move(p_right))
{
}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    // TODO
    if (left)
    {
        left->accept(p_visitor);
    }
    if (right)
    {
        right->accept(p_visitor);
    }
}
