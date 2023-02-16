#pragma once

#include <lox/box.hpp>
#include <lox/token.hpp>

#include <optional>
#include <variant>
#include <vector>

namespace lox {

// C++ is value-oriented whereas Java is reference-oriented.
// > You cannot overwrite a const object and refer to it by the same name
// > afterwards.
// https://stackoverflow.com/questions/58414966/move-construction-and-assignment-of-class-with-constant-member
//
// That said, it seems like this in possible in C++20 using
// std::{construct, destroy}_at.
//
// Therefore, I can't make the members of these structs const. That has a
// different meaning to what I'm intending. The problem is that you can't
// reassign a struct with const members (since const tells the compiler that
// that memory will never be overwritten).
struct literal_expr {
  value value;
};

struct variable_expr {
  token name;
};

using expr =
    std::variant<literal_expr, variable_expr, box<struct group_expr>,
                 box<struct assign_expr>, box<struct unary_expr>,
                 box<struct binary_expr>, box<struct conditional_expr>>;

struct group_expr {
  expr ex;
};

struct assign_expr {
  token name;
  expr  value;
};

struct unary_expr {
  token op;
  expr  right;
};

struct binary_expr {
  expr  left;
  token op;
  expr  right;
};

struct conditional_expr {
  // TODO: Should this keep track of tokens? How to implement properly?
  expr cond;
  expr conseq;
  expr alt;
};

struct expression_stmt {
  expr ex;
};

struct print_stmt {
  expr ex;
};

struct variable_stmt {
  token               name;
  std::optional<expr> init;
};

using stmt = std::variant<box<expression_stmt>, box<print_stmt>,
                          box<variable_stmt>, struct block_stmt>;

struct block_stmt {
  std::vector<stmt> stmts;
};

} // namespace lox
