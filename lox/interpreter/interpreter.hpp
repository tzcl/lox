#pragma once

#include <lox/ast.hpp>
#include <lox/interpreter/environment.hpp>
#include <lox/interpreter/value.hpp>
#include <lox/parser/token.hpp>

#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace lox {

struct interpreter {
  environment   env;
  std::ostream& output{std::cout};

  auto operator()(literal_expr const& e) -> value;
  auto operator()(variable_expr const& e) -> value;
  auto operator()(box<group_expr> const& e) -> value;
  auto operator()(box<assign_expr> const& e) -> value;
  auto operator()(box<unary_expr> const& e) -> value;
  auto operator()(box<logical_expr> const& e) -> value;
  auto operator()(box<binary_expr> const& e) -> value;
  auto operator()(box<conditional_expr> const& e) -> value;

  void operator()(expression_stmt const& s);
  void operator()(print_stmt const& s);
  void operator()(variable_stmt const& s);
  void operator()(block_stmt const& s);
  void operator()(box<if_stmt> const& s);
  void operator()(box<while_stmt> const& s);

  [[noreturn]] void operator()(break_stmt const& s);
};

void interpret(interpreter& interpreter, std::vector<stmt> const& stmts);

} // namespace lox
