#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,
                     ExpressionNode *p_expression,
                     CompoundStatementNode *p_compound_statement)
    : AstNode{line, col},
      expression(p_expression),
      compound_statement(p_compound_statement)
{
}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    // TODO
    if (expression)
    {
        expression->accept(p_visitor);
    }
    if (compound_statement)
    {
        compound_statement->accept(p_visitor);
    }
}
