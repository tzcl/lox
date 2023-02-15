#pragma once

#include <lox/environment.hpp>
#include <lox/expr.hpp>
#include <lox/token.hpp>

#include <string>
#include <variant>
#include <vector>

namespace lox {

struct interpreter {
  environment env;

  auto operator()(literal_expr const& e) -> value;
  auto operator()(variable_expr const& e) -> value;
  auto operator()(box<group_expr> const& e) -> value;
  auto operator()(box<assign_expr> const& e) -> value;
  auto operator()(box<unary_expr> const& e) -> value;
  auto operator()(box<binary_expr> const& e) -> value;
  auto operator()(box<conditional_expr> const& e) -> value;

  void operator()(box<expression_stmt> const& s);
  void operator()(box<print_stmt> const& s);
  void operator()(box<variable_stmt> const& s);
};

void interpret(interpreter& interpreter, std::vector<stmt> const& stmts);

} // namespace lox
