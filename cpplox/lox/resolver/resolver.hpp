#pragma once

#include <lox/interpreter/interpreter.hpp>
#include <lox/token/token.hpp>

#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace lox {

class resolver {
public:
  explicit resolver(interpreter& interpreter) : interpreter_(interpreter) {}

  void resolve(std::vector<stmt> const& stmts);

  void operator()(literal_expr const& e);
  void operator()(variable_expr const& e);
  void operator()(box<group_expr> const& e);
  void operator()(box<assign_expr> const& e);
  void operator()(box<unary_expr> const& e);
  void operator()(box<logical_expr> const& e);
  void operator()(box<binary_expr> const& e);
  void operator()(box<call_expr> const& e);
  void operator()(box<conditional_expr> const& e);

  void operator()(expression_stmt const& s);
  void operator()(print_stmt const& s);
  void operator()(variable_stmt const& s);
  void operator()(break_stmt const& s);
  void operator()(return_stmt const& s);
  void operator()(box<block_stmt> const& s);
  void operator()(box<function_stmt> const& s);
  void operator()(box<if_stmt> const& s);
  void operator()(box<while_stmt> const& s);

private:
  interpreter& interpreter_;

  std::deque<std::unordered_map<std::string, bool>> scopes{};

  void resolve_local(expr e, token name);
  void resolve_function(box<function_stmt> const& s);

  void begin_scope();
  void end_scope();

  void declare(token name);
  void define(token name);
};

} // namespace lox
