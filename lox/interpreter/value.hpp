#pragma once

#include <lox/errors.hpp>
#include <lox/token/token.hpp>

#include <string>
#include <variant>

namespace lox {

struct callable {
  friend auto operator==(const callable&, const callable&) -> bool = default;
};

// A value is either a literal or a callable. Instead of nesting variants,
// literal has been flattened out here.
using value = std::variant<std::monostate, bool, double, std::string, callable>;

using number_or_string = std::variant<double, std::string>;

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

} // namespace lox
