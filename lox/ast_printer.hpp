#pragma once

#include <lox/expr.hpp>
#include <lox/token.hpp>

#include <fmt/core.h>

#include <algorithm>
#include <string>
#include <vector>

namespace lox {

template <typename T>
concept printer = requires(T t, stmt const& s) { std::visit(t, s); };

auto print(printer auto p, std::vector<stmt> const& stmts)
    -> std::vector<std::string> {
  std::vector<std::string> output;
  std::ranges::transform(
      stmts, std::back_inserter(output),
      [&p](stmt s) -> std::string { return std::visit(p, s); });

  return output;
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
  auto operator()(box<expression_stmt> const& s) -> std::string {
    return fmt::format("(expr {})", std::visit(*this, s->ex));
  }
  auto operator()(box<print_stmt> const& s) -> std::string {
    return fmt::format("(print {})", std::visit(*this, s->ex));
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
  auto operator()(box<expression_stmt> const& s) -> std::string {
    return fmt::format("expr({})", std::visit(*this, s->ex));
  }
  auto operator()(box<print_stmt> const& s) -> std::string {
    return fmt::format("print({})", std::visit(*this, s->ex));
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
  auto operator()(box<expression_stmt> const& s) -> std::string {
    return fmt::format("{} expr", std::visit(*this, s->ex));
  }
  auto operator()(box<print_stmt> const& s) -> std::string {
    return fmt::format("{} print", std::visit(*this, s->ex));
  }
};

} // namespace lox
