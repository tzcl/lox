#pragma once

#include <lox/expr.hpp>
#include <lox/token.hpp>

#include <string>
#include <variant>
#include <vector>

namespace lox {

// TODO: This needs to store global state at some point
struct interpreter {
  auto operator()(literal_expr const& e) -> value;
  auto operator()(box<group_expr> const& e) -> value;
  auto operator()(box<unary_expr> const& e) -> value;
  auto operator()(box<binary_expr> const& e) -> value;
  auto operator()(box<conditional_expr> const& e) -> value;

  void operator()(box<expression_stmt> const& s);
  void operator()(box<print_stmt> const& s);
};

// TODO: Change this to take in an interpreter?
void interpret(std::vector<stmt> const& stmts);

} // namespace lox
