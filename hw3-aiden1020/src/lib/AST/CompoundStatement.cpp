#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col,
                                             std::vector<DeclNode *> p_decls,
                                             std::vector<AstNode *> p_stmts)
    : AstNode{line, col},
      var_decls(p_decls),
      stmts(p_stmts)
{
}

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {}

// void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    for (auto &decl : var_decls)
    {
        decl->accept(p_visitor);
    }
    for (auto &stmt : stmts)
    {
        stmt->accept(p_visitor);
    }
}