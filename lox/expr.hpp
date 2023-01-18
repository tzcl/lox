#pragma once

#include <lox/box.hpp>
#include <lox/token.hpp>

#include <variant>

namespace lox {

struct literal_expr {
  token_literal const value;
};

using expr = std::variant<literal_expr, box<struct binary_expr>,
                          box<struct grouping_expr>, box<struct unary_expr>>;

struct binary_expr {
  expr const left;
  token const op;
  expr const right;
};

struct grouping_expr {
  expr const ex;
};

struct unary_expr {
  token const op;
  expr const right;
};
} // namespace lox
