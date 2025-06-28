#include "AST/function.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col, std::string p_name,
                           std::vector<DeclNode *> p_decls,
                           std::string p_return_type,
                           CompoundStatementNode *const p_body)
    : AstNode{line, col},
      name(p_name),
      var_decls(p_decls),
      return_type(p_return_type),
      m_body(p_body)
{
}
// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    // TODO
    for (auto &decl : var_decls)
    {
        decl->accept(p_visitor);
    }
    if (m_body)
    {
        m_body->accept(p_visitor);
    }
}
