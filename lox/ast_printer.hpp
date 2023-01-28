#pragma once

#include <lox/expr.hpp>
#include <lox/token.hpp>

#include <fmt/core.h>

#include <string>

namespace lox {

template <typename T>
concept printer = requires(T t, expr const& e) { std::visit(t, e); };

auto print(printer auto p, expr const& e) -> std::string {
  return std::visit(p, e);
}

// Note, instead of declaring a struct, you can use a trivial struct (normally
// called overload) to let you pass lambdas to a visitor.

struct sexp_printer {
  auto operator()(literal_expr const& e) -> std::string {
    return fmt::format("{}", e.value);
  }
  auto operator()(box<group_expr> const& e) -> std::string {
    return fmt::format("(group {})", std::visit(*this, e->ex));
  }
  auto operator()(box<unary_expr> const& e) -> std::string {
    return fmt::format("({} {})", e->op.lexeme, std::visit(*this, e->right));
  }
  auto operator()(box<binary_expr> const& e) -> std::string {
    return fmt::format("({} {} {})", e->op.lexeme, std::visit(*this, e->left),
                       std::visit(*this, e->right));
  }
  auto operator()(box<conditional_expr> const& e) -> std::string {
    return fmt::format("(if {} ({}) ({}))", std::visit(*this, e->cond),
                       std::visit(*this, e->conseq), std::visit(*this, e->alt));
  }
};

struct ast_printer {
  auto operator()(literal_expr const& e) -> std::string {
    return fmt::format("{}", e.value);
  }
  auto operator()(box<group_expr> const& e) -> std::string {
    return fmt::format("(group {})", std::visit(*this, e->ex));
  }
  auto operator()(box<unary_expr> const& e) -> std::string {
    return fmt::format("({} {})", e->op.lexeme, std::visit(*this, e->right));
  }
  auto operator()(box<binary_expr> const& e) -> std::string {
    return fmt::format("({} {} {})", std::visit(*this, e->left), e->op.lexeme,
                       std::visit(*this, e->right));
  }
  auto operator()(box<conditional_expr> const& e) -> std::string {
    return fmt::format("(if {} then {} else {})", std::visit(*this, e->cond),
                       std::visit(*this, e->conseq), std::visit(*this, e->alt));
  }
};

struct rpn_printer {
  auto operator()(literal_expr const& e) -> std::string {
    return fmt::format("{}", e.value);
  }
  auto operator()(box<group_expr> const& e) -> std::string {
    return fmt::format("{}", std::visit(*this, e->ex));
  }
  auto operator()(box<unary_expr> const& e) -> std::string {
    return fmt::format("{} {}", std::visit(*this, e->right), e->op.lexeme);
  }
  auto operator()(box<binary_expr> const& e) -> std::string {
    return fmt::format("{} {} {}", std::visit(*this, e->left),
                       std::visit(*this, e->right), e->op.lexeme);
  }
  auto operator()(box<conditional_expr> const& e) -> std::string {
    return fmt::format("{} {} {} if", std::visit(*this, e->conseq),
                       std::visit(*this, e->alt), std::visit(*this, e->cond));
  }
};

} // namespace lox
