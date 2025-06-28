#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <vector>
#include <string>
#include <memory>

class DeclNode : public AstNode
{
public:
  // variable declaration
  DeclNode(const uint32_t line, const uint32_t col,
           std::vector<VariableNode *> p_identifiers,
           std::string p_type
           /* TODO: identifiers, type */);

  // constant variable declaration
  DeclNode(const uint32_t, const uint32_t col,
           std::vector<VariableNode *> p_identifiers,
           ConstantValueNode *const p_constant_value
           /* TODO: identifiers, constant */);

  ~DeclNode() = default;

  void print() override;
  const std::vector<VariableNode *> &getIdentifiers() const { return identifiers; }
  const char *getTypeCString() const { return type.c_str(); }
  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: variables
  std::vector<VariableNode *> identifiers;
  std::string type;
  ConstantValueNode *constant;
};

#endif
