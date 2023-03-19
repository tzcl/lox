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

using namespace std::string_literals;

struct ast_printer {
  int indent = 0;

  auto operator()(const literal_expr& e) -> std::string {
    return fmt::format("{}", e.literal);
  }

  auto operator()(const variable_expr& e) -> std::string {
    return fmt::format("{}", e.name.lexeme);
  }

  auto operator()(const box<group_expr>& e) -> std::string {
    return fmt::format("group: {}", std::visit(*this, e->ex));
  }

  auto operator()(const box<assign_expr>& e) -> std::string {
    return fmt::format("{} = {}", e->name.lexeme, std::visit(*this, e->value));
  }

  auto operator()(const box<unary_expr>& e) -> std::string {
    return fmt::format("({}{})", e->op.lexeme, std::visit(*this, e->right));
  }

  auto operator()(const box<logical_expr>& e) -> std::string {
    return fmt::format("({} {} {})", std::visit(*this, e->left), e->op.lexeme,
                       std::visit(*this, e->right));
  }

  auto operator()(const box<binary_expr>& e) -> std::string {
    return fmt::format("({} {} {})", std::visit(*this, e->left), e->op.lexeme,
                       std::visit(*this, e->right));
  }

  auto operator()(const box<call_expr>& e) -> std::string {
    std::vector<std::string> args(std::size(e->args));
    std::ranges::transform(e->args, std::begin(args), [this](const expr& ex) {
      return std::visit(*this, ex);
    });
    return fmt::format("{}({})", std::visit(*this, e->callee),
                       fmt::join(args, ","));
  }

  auto operator()(const box<conditional_expr>& e) -> std::string {
    return fmt::format("(if {} then {} else {})", std::visit(*this, e->cond),
                       std::visit(*this, e->then), std::visit(*this, e->alt));
  }

  auto operator()(const expression_stmt& s) -> std::string {
    return fmt::format("expr: {}", std::visit(*this, s.ex));
  }

  auto operator()(const print_stmt& s) -> std::string {
    return fmt::format("print: {}", std::visit(*this, s.ex));
  }

  auto operator()(const variable_stmt& s) -> std::string {
    return fmt::format("var {} = {}", s.name.lexeme,
                       s.init ? std::visit(*this, *s.init) : "nil"s);
  }

  auto operator()(const return_stmt& s) -> std::string {
    return fmt::format("return {}",
                       s.value ? std::visit(*this, *s.value) : "nil"s);
  }

  auto operator()(const break_stmt& s) -> std::string {
    return fmt::format("break (depth: {})", s.loop_depth);
  }

  auto operator()(const block_stmt& s) -> std::string {
    std::vector<std::string> stmts(std::size(s.stmts));
    std::ranges::transform(s.stmts, std::begin(stmts), [this](const stmt& ss) {
      return fmt::format("{:{}}{}", "", this->indent + 2,
                         std::visit(ast_printer{this->indent + 2}, ss));
    });
    return fmt::format("{{\n{}\n{:{}}}}", fmt::join(stmts, "\n"), "",
                       this->indent);
  }

  auto operator()(const function_stmt& s) -> std::string {
    return fmt::format("<fn {}>", s.name.lexeme);
  }

  auto operator()(const box<if_stmt>& s) -> std::string {
    if (s->alt) {
      return fmt::format("if ({}) {} else {}", std::visit(*this, s->cond),
                         std::visit(*this, s->then),
                         std::visit(*this, *s->alt));
    } else {
      return fmt::format("if ({}) {}", std::visit(*this, s->cond),
                         std::visit(*this, s->then));
    }
  }

  auto operator()(const box<while_stmt>& s) -> std::string {
    return fmt::format("while ({}) {}", std::visit(*this, s->cond),
                       std::visit(*this, s->body));
  }
};

} // namespace lox
