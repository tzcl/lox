#pragma once

#include <lox/expr.hpp>
#include <lox/token.hpp>

#include <fmt/core.h>

#include <string>

namespace lox {

template <typename T>
concept printer = requires(T t, expr const &e) {
  std::visit(t, e);
};

auto print(printer auto p, expr const &e) -> std::string {
  return std::visit(p, e);
}

// You can use a trivial struct (called overload) to let you pass
// lambdas to a visitor.

struct ast_printer {
  auto operator()(literal_expr const &e) -> std::string {
    return fmt::format("{}", e.value);
  }
  auto operator()(box<binary_expr> const &e) -> std::string {
    return fmt::format("({} {} {})", e->op.lexeme, std::visit(*this, e->left),
                       std::visit(*this, e->right));
  }
  auto operator()(box<grouping_expr> const &e) -> std::string {
    return fmt::format("(group {})", std::visit(*this, e->ex));
  }
  auto operator()(box<unary_expr> const &e) -> std::string {
    return fmt::format("({} {})", e->op.lexeme, std::visit(*this, e->right));
  }
};

} // namespace lox
