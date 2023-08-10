#pragma once

#include <lox/ast/ast.hpp>
#include <lox/box.hpp>
#include <lox/errors.hpp>
#include <lox/token/token.hpp>

#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace lox {

// A value is either a literal or a callable. Instead of nesting variants,
// literal has been flattened out here.
// TODO: Simplify this into just one literal variant
using value = std::variant<std::monostate, bool, double, std::string,
                           box<struct function>, box<struct builtin>>;

struct callable {
  std::vector<token> const& params;
  std::vector<value> const& args;
  std::vector<stmt> const&  body;
};

using env_ptr = std::shared_ptr<class environment>;

using interpret_func = std::function<value(callable, env_ptr)>;

struct function {
  function_stmt decl;
  env_ptr       enclosing;

  [[nodiscard]] auto call(interpret_func const&     fn,
                          std::vector<value> const& args) const -> value {
    return fn(callable{decl.params, args, decl.body}, enclosing);
  }

  friend auto operator==(function const& a, function const& b) -> bool {
    return a.decl.name.lexeme == b.decl.name.lexeme;
  }
};

struct builtin {
  std::string                              name;
  int                                      arity;
  std::function<value(std::vector<value>)> fn;

  friend auto operator==(builtin const& a, builtin const& b) -> bool {
    return a.name == b.name;
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
auto equal(token token, value left, value right) -> bool;
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
auto call(token paren, value callee, std::vector<value> const& args,
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
