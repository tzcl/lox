#pragma once

#include <lox/ast/ast.hpp>
#include <lox/box.hpp>
#include <lox/errors.hpp>
#include <lox/token/token.hpp>

#include <string>
#include <variant>
#include <vector>

namespace lox {

// A value is either a literal or a callable. Instead of nesting variants,
// literal has been flattened out here.
using value =
    std::variant<std::monostate, bool, double, std::string, struct function>;

// TODO: Could move this into a separate interface
struct callable {
  const std::vector<token>& params;
  const std::vector<value>& args;
  const std::vector<stmt>&  body;
};

using interpret_func = auto(*)(callable) -> value;

struct function {
  function_stmt decl;

  auto call(interpret_func fn, const std::vector<value>& args) const -> value {
    return fn(callable{decl.params, args, decl.body});
  }

  friend auto operator==(const function& a, const function& b) -> bool {
    return a.decl.name.lexeme == b.decl.name.lexeme;
  }
};

using number_or_string = std::variant<double, std::string>;

namespace values {

// *** Operations ***

auto to_string(value value) -> std::string;
auto to_value(literal literal) -> value;
auto to_value(number_or_string result) -> value;

// Unary operations
auto is_truthy(value value) -> bool;
auto negate(token token, value value) -> double;

// Binary operations
// - Comparison operations
auto less_than(token token, value left, value right) -> bool;
auto greater_than(token token, value left, value right) -> bool;
auto less_equal(token token, value left, value right) -> bool;
auto greater_equal(token token, value left, value right) -> bool;

// - Maths operations
auto plus(token token, value left, value right) -> number_or_string;
auto minus(token token, value left, value right) -> double;
auto multiply(token token, value left, value right) -> number_or_string;
auto divide(token token, value left, value right) -> double;

// Function call
auto call(token paren, value callee, const std::vector<value>& args,
          interpret_func fn) -> value;
auto arity(token paren, value callee) -> int;

} // namespace values

} // namespace lox

template <>
struct fmt::formatter<lox::value> : formatter<std::string> {
  template <typename FormatContext>
  auto format(lox::value const& value, FormatContext& ctx) const {
    return formatter<std::string>::format(lox::values::to_string(value), ctx);
  }
};
