#pragma once

#include <lox/ast/ast.hpp>
#include <lox/interpreter/environment.hpp>
#include <lox/interpreter/value.hpp>
#include <lox/token/token.hpp>

#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace lox {

struct interpreter {
  environment   env{nullptr};
  std::ostream& output = std::cout;

  auto operator()(const literal_expr& e) -> value;
  auto operator()(const variable_expr& e) -> value;
  auto operator()(const box<group_expr>& e) -> value;
  auto operator()(const box<assign_expr>& e) -> value;
  auto operator()(const box<unary_expr>& e) -> value;
  auto operator()(const box<logical_expr>& e) -> value;
  auto operator()(const box<binary_expr>& e) -> value;
  auto operator()(const box<conditional_expr>& e) -> value;

  void operator()(const expression_stmt& s);
  void operator()(const print_stmt& s);
  void operator()(const variable_stmt& s);
  void operator()(const block_stmt& s);
  void operator()(const box<if_stmt>& s);
  void operator()(const box<while_stmt>& s);

  [[noreturn]] void operator()(break_stmt const& s);
};

void interpret(interpreter& interpreter, std::vector<stmt> const& stmts);

} // namespace lox
