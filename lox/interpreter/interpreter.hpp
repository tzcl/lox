#pragma once

#include <lox/ast/ast.hpp>
#include <lox/interpreter/environment.hpp>
#include <lox/interpreter/value.hpp>
#include <lox/token/token.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace lox {

class interpreter {
public:
  explicit interpreter(std::ostream& output = std::cout);

  void interpret(std::vector<stmt> const& stmts);
  void resolve(expr ex, int depth);

  auto operator()(literal_expr const& e) -> value;
  auto operator()(variable_expr const& e) -> value;
  auto operator()(box<group_expr> const& e) -> value;
  auto operator()(box<assign_expr> const& e) -> value;
  auto operator()(box<unary_expr> const& e) -> value;
  auto operator()(box<logical_expr> const& e) -> value;
  auto operator()(box<binary_expr> const& e) -> value;
  auto operator()(box<call_expr> const& e) -> value;
  auto operator()(box<conditional_expr> const& e) -> value;

  void operator()(expression_stmt const& s);
  void operator()(print_stmt const& s);
  void operator()(variable_stmt const& s);
  void operator()(box<block_stmt> const& s);
  void operator()(box<function_stmt> const& s);
  void operator()(box<if_stmt> const& s);
  void operator()(box<while_stmt> const& s);

  [[noreturn]] void operator()(break_stmt const& s);
  [[noreturn]] void operator()(return_stmt const& s);

  auto lookup_var(token name, expr ex) -> value;
  void assign_var(token name, expr ex, value value);

private:
  env_ptr       globals_;
  env_ptr       env_;
  std::ostream& output_;

  std::unordered_map<std::string, int> locals{};

  // TODO: This feels hacky
  auto interpret(callable callable, env_ptr const& closure) -> value;
};

} // namespace lox
