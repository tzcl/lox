#pragma once

#include <lox/expr.hpp>
#include <lox/token.hpp>

#include <string>
#include <variant>

namespace lox {

// TODO: This needs to store global state at some point
struct interpreter {
  auto operator()(literal_expr const& e) -> value;
  auto operator()(box<group_expr> const& e) -> value;
  auto operator()(box<unary_expr> const& e) -> value;
  auto operator()(box<binary_expr> const& e) -> value;
  auto operator()(box<conditional_expr> const& e) -> value;
};

// TODO: Change this to take in an interpreter?
void interpret(expr ex);

} // namespace lox
