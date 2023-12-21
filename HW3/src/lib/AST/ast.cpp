#include <AST/ast.hpp>

AstNode::~AstNode() {}

AstNode::AstNode(const uint32_t line, const uint32_t col)
    : location(line, col) {}

const Location &AstNode::getLocation() const { return location; }

// Done