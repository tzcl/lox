#pragma once

#include <lox/box.hpp>
#include <lox/token.hpp>

#include <variant>

namespace lox {

// C++ is value-oriented whereas Java is reference-oriented.
// > You cannot overwrite a const object and refer to it by the same name
// > afterwards.
// https://stackoverflow.com/questions/58414966/move-construction-and-assignment-of-class-with-constant-member
//
// That said, tt seems like this in possible in C++20 using
// std::{construct, destroy}_at.
struct literal_expr {
  token_literal value;
};

using expr =
    std::variant<literal_expr, box<struct group_expr>, box<struct unary_expr>,
                 box<struct binary_expr>, box<struct ternary_expr>>;

struct group_expr {
  expr ex;
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

// TODO: Can this be anything other than a conditional?
struct ternary_expr {
  expr  cond;
  token hook;
  expr  conseq;
  token colon;
  expr  alt;
};

} // namespace lox
