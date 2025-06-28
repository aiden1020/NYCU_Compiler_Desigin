#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
                 DeclNode *p_declaration,
                 AssignmentNode *p_assignment,
                 ExpressionNode *p_expression,
                 CompoundStatementNode *p_compound_statement)
    : AstNode{line, col},
      declaration(p_declaration),
      assignment(p_assignment),
      expression(p_expression),
      compound_statement(p_compound_statement)
{
}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    // TODO
    if (declaration)
    {
        declaration->accept(p_visitor);
    }
    if (assignment)
    {
        assignment->accept(p_visitor);
    }
    if (expression)
    {
        expression->accept(p_visitor);
    }
    if (compound_statement)
    {
        compound_statement->accept(p_visitor);
    }
}
