#ifndef AST_UTILS_H
#define AST_UTILS_H

#include "AST/ast.hpp"

#include <cstdint>
#include <string>
using namespace std;

struct IdInfo {
    Location location;
    string id;

    IdInfo(const uint32_t line, const uint32_t col, const char * const p_id)
        : location(line, col), id(p_id) {}
};

#endif
