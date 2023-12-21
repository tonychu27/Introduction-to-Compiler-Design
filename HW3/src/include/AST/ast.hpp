#ifndef AST_AST_NODE_H
#define AST_AST_NODE_H

#include <cstdint>
#include <vector>
#include <string>
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

class AstNodeVisitor;

struct Location {
    uint32_t line;
    uint32_t col;

    ~Location() = default;
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}
};

class Unary{
public:
  bool neg = false;
  bool NOT = false;
};

class Binary{
public:
  bool mul = false;
  bool div = false;
  bool mod = false;

  bool add = false;
  bool sub = false;

  bool sm = false;
  bool sme = false;
  bool eq = false;
  bool ne = false;
  bool lge = false;
  bool lg = false;

  bool AND = false;
  bool OR = false;
};

class ConstValue{
public:
  uint32_t line;
  uint32_t col;
  
  int intValue;
  float realValue;
  char *strValue;

  bool intType = false;
  bool realType = false;
  bool strType = false;
  bool boolType = false;
};

class Name{
public:
  char *id;
  uint32_t line;
  uint32_t col;
};

class AstNode {
  public:
    virtual ~AstNode() = 0;
    AstNode(const uint32_t line, const uint32_t col);

    const Location &getLocation() const;

    virtual void print() = 0;
    virtual void accept(AstNodeVisitor &p_visitor) = 0;

  protected:
    Location location;

};

#endif
