#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
                               VariableReferenceNode *p_variable_ref,
                               ExpressionNode *p_expression)
    : AstNode{line, col},
      variable_ref(p_variable_ref),
      expression(p_expression)
{
}

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    // TODO
    if (variable_ref)
    {
        variable_ref->accept(p_visitor);
    }
    if (expression)
    {
        expression->accept(p_visitor);
    }
}
