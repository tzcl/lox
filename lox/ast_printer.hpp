#pragma once

#include <lox/expr.hpp>

#include <fmt/std.h>
#include <string>

namespace lox {

template <typename T>
concept printer = requires(T t, const expr &e) {
  std::visit(t, e);
};

auto print(printer auto p, const expr &e) -> std::string {
  return std::visit(p, e);
}

// You can use a trivial struct (called overload) to let you pass
// lambdas to a visitor.

struct ast_printer {
  auto operator()(const literal_expr &e) -> std::string {
    return fmt::format("{}", e.value);
  }
  auto operator()(const box<binary_expr> &e) -> std::string {
    return fmt::format("({} {} {})", e->op.lexeme_, std::visit(*this, e->left),
                       std::visit(*this, e->right));
  }
  auto operator()(const box<grouping_expr> &e) -> std::string {
    return fmt::format("(group {})", std::visit(*this, e->ex));
  }
  auto operator()(const box<unary_expr> &e) -> std::string {
    return fmt::format("({} {})", e->op.lexeme_, std::visit(*this, e->right));
  }
};

} // namespace lox
