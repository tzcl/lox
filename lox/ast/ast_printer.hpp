#pragma once

#include <lox/ast/ast.hpp>
#include <lox/token/token.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <string>
#include <vector>

namespace lox {

template <typename T>
concept stmt_visitor = requires(T t, stmt const& s) { std::visit(t, s); };

auto print(stmt_visitor auto p, std::vector<stmt> const& stmts)
    -> std::vector<std::string> {
  std::vector<std::string> output;
  std::ranges::transform(
      stmts, std::back_inserter(output),
      [&p](stmt s) -> std::string { return std::visit(p, s); });

  return output;
}

struct ast_printer {
  int indent = 0;

  auto operator()(literal_expr const& e) -> std::string {
    return fmt::format("{}", e.value);
  }
  auto operator()(variable_expr const& e) -> std::string {
    return fmt::format("{}", e.name.lexeme);
  }
  auto operator()(box<group_expr> const& e) -> std::string {
    return fmt::format("group: {}", std::visit(*this, e->ex));
  }
  auto operator()(box<assign_expr> const& e) -> std::string {
    return fmt::format("{} = {}", e->name.lexeme, std::visit(*this, e->value));
  }
  auto operator()(box<unary_expr> const& e) -> std::string {
    return fmt::format("({}{})", e->op.lexeme, std::visit(*this, e->right));
  }
  auto operator()(box<logical_expr> const& e) -> std::string {
    return fmt::format("({} {} {})", std::visit(*this, e->left), e->op.lexeme,
                       std::visit(*this, e->right));
  }
  auto operator()(box<binary_expr> const& e) -> std::string {
    return fmt::format("({} {} {})", std::visit(*this, e->left), e->op.lexeme,
                       std::visit(*this, e->right));
  }
  auto operator()(box<conditional_expr> const& e) -> std::string {
    return fmt::format("(if {} then {} else {})", std::visit(*this, e->cond),
                       std::visit(*this, e->then), std::visit(*this, e->alt));
  }
  auto operator()(expression_stmt const& s) -> std::string {
    return fmt::format("expr: {}", std::visit(*this, s.ex));
  }
  auto operator()(print_stmt const& s) -> std::string {
    return fmt::format("print: {}", std::visit(*this, s.ex));
  }
  auto operator()(variable_stmt const& s) -> std::string {
    using namespace std::string_literals;
    return fmt::format("var {} = {}", s.name.lexeme,
                       s.init ? std::visit(*this, *s.init) : "nil"s);
  }
  auto operator()(break_stmt const& s) -> std::string {
    using namespace std::string_literals;
    return fmt::format("break (depth: {})", s.loop_depth);
  }
  auto operator()(block_stmt const& s) -> std::string {
    std::vector<std::string> stmts(std::size(s.stmts));
    std::ranges::transform(s.stmts, std::begin(stmts), [this](const stmt& ss) {
      return fmt::format("{:{}}{}", "", this->indent + 2,
                         std::visit(ast_printer{this->indent + 2}, ss));
    });
    return fmt::format("{{\n{}\n{:{}}}}", fmt::join(stmts, "\n"), "",
                       this->indent);
  }
  auto operator()(box<if_stmt> const& s) -> std::string {
    if (s->alt) {
      return fmt::format("if ({}) {} else {}", std::visit(*this, s->cond),
                         std::visit(*this, s->then),
                         std::visit(*this, *s->alt));
    } else {
      return fmt::format("if ({}) {}", std::visit(*this, s->cond),
                         std::visit(*this, s->then));
    }
  }
  auto operator()(box<while_stmt> const& s) -> std::string {
    return fmt::format("while ({}) {}", std::visit(*this, s->cond),
                       std::visit(*this, s->body));
  }
};

} // namespace lox
