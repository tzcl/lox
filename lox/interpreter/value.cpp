#include <lox/errors.hpp>
#include <lox/interpreter/value.hpp>

#include <fmt/core.h>

#include <optional>
#include <utility>

namespace lox::values {

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
                 [](const function& f) {
                   return fmt::format("<fn {}>", f.decl.name.lexeme);
                 }},
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

  auto operator()(double arg) const -> double { return -arg; }
  auto operator()(auto&) const -> double {
    throw runtime_error(token, "operand must be a number");
  }
};

auto negate(token token, value value) -> double {
  return std::visit(negate_visitor{std::move(token)}, value);
}

struct less_than_visitor {
  token token;

  auto operator()(double left, double right) const -> bool {
    return left < right;
  }
  auto operator()(const std::string& left, const std::string& right) const
      -> bool {
    return left < right;
  }
  auto operator()(auto&, auto&) const -> bool {
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

  auto operator()(double left, double right) const -> number_or_string {
    return left + right;
  }
  auto operator()(std::string left, std::string right) const
      -> number_or_string {
    return left + right;
  }
  auto operator()(double left, std::string right) const -> number_or_string {
    return fmt::format("{}{}", left, right);
  }
  auto operator()(std::string left, double right) const -> number_or_string {
    return fmt::format("{}{}", left, right);
  }
  auto operator()(auto&, auto&) const -> number_or_string {
    throw runtime_error(token, "operands must be numbers or strings");
  }
};

auto plus(token token, value left, value right) -> number_or_string {
  return std::visit(plus_visitor{std::move(token)}, left, right);
}

struct minus_visitor {
  token token;

  auto operator()(double left, double right) const -> double {
    return left - right;
  }
  auto operator()(auto&, auto&) const -> double {
    throw runtime_error(std::move(token), "operands must be two numbers");
  }
};

auto minus(token token, value left, value right) -> double {
  return std::visit(minus_visitor{std::move(token)}, left, right);
}

struct multiply_visitor {
  token token;

  auto operator()(double left, double right) const -> number_or_string {
    return left * right;
  }
  auto operator()(double left, std::string right) const -> number_or_string {
    int length = static_cast<int>(left);

    std::string result;
    while (--length >= 0) result += right;

    return result;
  }
  auto operator()(std::string left, double right) const -> number_or_string {
    int length = static_cast<int>(right);

    std::string result;
    while (--length >= 0) result += left;

    return result;
  }
  auto operator()(auto&, auto&) const -> number_or_string {
    throw runtime_error(
        token, "operands must be two numbers or a number and a string");
  }
};

auto multiply(token token, value left, value right) -> number_or_string {
  return std::visit(multiply_visitor{std::move(token)}, left, right);
}

struct divide_visitor {
  token token;

  auto operator()(double left, double right) const -> double {
    if (right <= EPSILON) throw runtime_error(token, "division by zero");
    return left / right;
  }
  auto operator()(auto&, auto&) const -> double {
    throw runtime_error(token, "operands must be two numbers");
  }
};

auto divide(token token, value left, value right) -> double {
  return std::visit(divide_visitor{std::move(token)}, left, right);
}

struct call_visitor {
  token                     paren;
  const std::vector<value>& args;
  interpret_func            interpret;

  auto operator()(const function& fn) const -> value {
    fn.call(interpret, args);
    // TODO: Return a proper value
    return {};
  }
  auto operator()(const auto&) const -> value {
    throw runtime_error(paren,
                        "call visitor: can only call functions and classes");
  }
};

auto call(token paren, value callee, const std::vector<value>& args,
          interpret_func fn) -> value {
  fmt::print("index: {}\n", callee.index());
  return std::visit(call_visitor{std::move(paren), args, fn}, callee);
}

struct arity_visitor {
  token paren;

  auto operator()(const function& fn) const -> int {
    return static_cast<int>(std::ssize(fn.decl.params));
  }
  auto operator()(const auto&) const -> int {
    throw runtime_error(paren,
                        "arity visitor: can only call functions and classes");
  }
};

auto arity(token paren, value callee) -> int {
  return std::visit(arity_visitor{std::move(paren)}, callee);
}

} // namespace lox::values
