#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line, const uint32_t col,
                                             std::string p_name,
                                             std::vector<ExpressionNode *> p_expressions)
    : ExpressionNode{line, col},
      name(p_name),
      expressions(p_expressions)
{
}

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col, std::string p_name)
    : ExpressionNode{line, col},
      name(p_name) {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    // TODO
    for (auto &expr : expressions)
    {
        expr->accept(p_visitor);
    }
}
