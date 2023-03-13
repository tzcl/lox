#pragma once

#include <lox/ast.hpp>
#include <lox/box.hpp>

#include <variant>
#include <vector>

namespace lox {

using value = std::variant<literal, struct callable>;

auto to_string(value value) -> std::string;
auto is_truthy(value value) -> bool;

auto get_literal(token token, value value) -> literal;

template <typename T>
concept visitor =
    requires(T& t, const std::vector<stmt>& stmts) { interpret(t, stmts); };

struct callable {
  function_stmt decl;

  void call(visitor auto visitor, const std::vector<value>& values);
  auto arity() -> int;
};

} // namespace lox

template <>
struct fmt::formatter<lox::value> : formatter<std::string> {
  template <typename FormatContext>
  auto format(lox::value const& value, FormatContext& ctx) const {
    return formatter<std::string>::format(lox::to_string(value), ctx);
  }
};
