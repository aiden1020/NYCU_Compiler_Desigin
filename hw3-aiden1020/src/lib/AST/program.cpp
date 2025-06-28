#include "AST/program.hpp"
#include <iostream>
// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const std::string p_name,
                         std::vector<DeclNode *> p_decls,
                         std::vector<FunctionNode *> p_func_decls,
                         CompoundStatementNode *const p_body)
    : AstNode{line, col},
      name(p_name),
      var_decls(p_decls),
      func_decls(p_func_decls),
      m_body(p_body)
{
}

// visitor pattern version: const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::print()
{
    // TODO
    // outputIndentationSpace();

    std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");

    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();
}

// void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version
//     /* TODO
//      *
//      * for (auto &decl : var_decls) {
//      *     decl->accept(p_visitor);
//      * }
//      *
//      * // functions
//      *
//      * body->accept(p_visitor);
//      */
// }
void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    for (auto &decl : var_decls)
    {
        decl->accept(p_visitor);
    }
    for (auto &func : func_decls)
    {
        func->accept(p_visitor);
    }
    m_body->accept(p_visitor);
}