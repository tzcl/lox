#include <lox/errors.hpp>
#include <lox/interpreter/value.hpp>

#include <fmt/core.h>

#include <optional>
#include <utility>

namespace lox {

const static double EPSILON = 1e-10;

// clang-format off
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; }; 
// Still need deduction guide with Clang 15
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
// clang-format on

auto to_string(value value) -> std::string {
  using namespace std::string_literals;
  return std::visit(
      overloaded{[](const std::monostate&) { return "nil"s; },
                 [](bool arg) { return arg ? "true"s : "false"s; },
                 [](double arg) { return fmt::format("{}", arg); },
                 [](const std::string& arg) { return arg; },
                 [](const callable&) { return "callable"s; }},
      value);
}

auto to_value(literal literal) -> value {
  return std::visit(overloaded{[](auto arg) { return value{arg}; }}, literal);
}

auto to_value(number_or_string result) -> value {
  return std::visit(overloaded{[](auto arg) { return value{arg}; }}, result);
}

auto is_truthy(value value) -> bool {
  return std::visit(overloaded{
                        [](const std::monostate&) { return false; },
                        [](bool arg) { return arg; },
                        [](auto) { return true; },
                    },
                    value);
}

// Can't throw exceptions using overloaded trick.
struct negate_visitor {
  token token;

  auto operator()(double arg) -> double { return -arg; }
  auto operator()(auto&) -> double {
    throw runtime_error(token, "operand must be a number");
  }
};

auto negate(token token, value value) -> double {
  return std::visit(negate_visitor{std::move(token)}, value);
}

struct less_than_visitor {
  token token;

  auto operator()(double left, double right) -> bool { return left < right; }
  auto operator()(const std::string& left, const std::string& right) -> bool {
    return left < right;
  }
  auto operator()(auto&, auto&) -> bool {
    throw runtime_error(token, "operands must be two numbers or two strings");
  }
};

auto less_than(token token, value left, value right) -> bool {
  return std::visit(less_than_visitor{std::move(token)}, left, right);
}

auto greater_than(token token, value left, value right) -> bool {
  return less_than(std::move(token), std::move(right), std::move(left));
}

auto less_equal(token token, value left, value right) -> bool {
  return !(greater_than(std::move(token), std::move(left), std::move(right)));
}

auto greater_equal(token token, value left, value right) -> bool {
  return !(less_than(std::move(token), std::move(left), std::move(right)));
}

struct plus_visitor {
  token token;

  auto operator()(double left, double right) -> number_or_string {
    return left + right;
  }
  auto operator()(std::string left, std::string right) -> number_or_string {
    return left + right;
  }
  auto operator()(double left, std::string right) -> number_or_string {
    return fmt::format("{}{}", left, right);
  }
  auto operator()(std::string left, double right) -> number_or_string {
    return fmt::format("{}{}", left, right);
  }
  auto operator()(auto&, auto&) -> number_or_string {
    throw runtime_error(token, "operands must be numbers or strings");
  }
};

auto plus(token token, value left, value right) -> number_or_string {
  return std::visit(plus_visitor{std::move(token)}, left, right);
}

struct minus_visitor {
  token token;

  auto operator()(double left, double right) -> double { return left - right; }
  auto operator()(auto&, auto&) -> double {
    throw runtime_error(std::move(token), "operands must be two numbers");
  }
};

auto minus(token token, value left, value right) -> double {
  return std::visit(minus_visitor{std::move(token)}, left, right);
}

struct multiply_visitor {
  token token;

  auto operator()(double left, double right) -> number_or_string {
    return left * right;
  }
  auto operator()(double left, std::string right) -> number_or_string {
    int length = static_cast<int>(left);

    std::string result;
    while (--length >= 0) result += right;

    return result;
  }
  auto operator()(std::string left, double right) -> number_or_string {
    int length = static_cast<int>(right);

    std::string result;
    while (--length >= 0) result += left;

    return result;
  }
  auto operator()(auto&, auto&) -> number_or_string {
    throw runtime_error(
        token, "operands must be two numbers or a number and a string");
  }
};

auto multiply(token token, value left, value right) -> number_or_string {
  return std::visit(multiply_visitor{std::move(token)}, left, right);
}

struct divide_visitor {
  token token;

  auto operator()(double left, double right) -> double {
    if (right <= EPSILON) throw runtime_error(token, "division by zero");
    return left / right;
  }
  auto operator()(auto&, auto&) -> double {
    throw runtime_error(std::move(token), "operands must be two numbers");
  }
};

auto divide(token token, value left, value right) -> double {
  return std::visit(divide_visitor{std::move(token)}, left, right);
}

} // namespace lox
