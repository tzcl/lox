#pragma once

#include <lox/grammar.hpp>
#include <lox/token.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

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

struct ast_printer {
  auto operator()(literal_expr const& e) -> std::string {
    return fmt::format("{}", e.value);
  }
  auto operator()(variable_expr const& e) -> std::string {
    return fmt::format("{}", e.name.lexeme);
  }
  auto operator()(box<group_expr> const& e) -> std::string {
    return fmt::format("(group {})", std::visit(*this, e->ex));
  }
  auto operator()(box<assign_expr> const& e) -> std::string {
    return fmt::format("({} := {})", e->name.lexeme,
                       std::visit(*this, e->value));
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
  auto operator()(box<variable_stmt> const& s) -> std::string {
    using namespace std::string_literals;
    return fmt::format("{}:{}", s->name.lexeme,
                       s->init ? std::visit(*this, *s->init) : "nil"s);
  }
  auto operator()(block_stmt const& s) -> std::string {
    std::vector<std::string> stmts(std::size(s.stmts));
    std::ranges::transform(s.stmts, std::begin(stmts),
                           [this](stmt ss) { return std::visit(*this, ss); });
    return fmt::format("{{ {} }}", fmt::join(stmts, " "));
  }
};

} // namespace lox
