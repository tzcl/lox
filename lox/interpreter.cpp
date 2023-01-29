#include <lox/errors.hpp>
#include <lox/interpreter.hpp>

#include <functional>
#include <utility>

namespace lox {

using enum token_type;

// clang-format off
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; }; 
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
// clang-format on

static auto is_truthy(value val) -> bool {
  return std::visit(overloaded{[](std::monostate&) { return false; },
                               [](bool arg) { return arg; },
                               [](auto&&) { return true; }},
                    val);
}

template <typename T>
static auto check_operand(token op, value val) -> T {
  try {
    return std::get<T>(val);
  } catch (std::bad_variant_access&) {
    throw errors::runtime_error(std::move(op), "operand must be a number");
  }
}

template <typename T, typename Op>
static auto check_operands(token token, value left, Op op, value right) -> T {
  try {
    return op(std::get<T>(left), std::get<T>(right));
  } catch (std::bad_variant_access&) {
    throw errors::runtime_error(std::move(token), "operands must be numbers");
  }
}

auto interpreter::operator()(literal_expr const& e) -> value { return e.value; }
auto interpreter::operator()(box<group_expr> const& e) -> value {
  return std::visit(*this, e->ex);
}
auto interpreter::operator()(box<unary_expr> const& e) -> value {
  value right = std::visit(*this, e->right);

  switch (e->op.type) {
  case BANG:
    return !is_truthy(right);
  case MINUS:
    return -check_operand<double>(e->op, right);
  default:
    break;
  }

  __builtin_unreachable();
}

auto interpreter::operator()(box<binary_expr> const& e) -> value {
  // Here is an important semantic choice: we evaluate the LHS before the RHS.
  // Also, we evaluate both operands before checking their types are valid.
  value left  = std::visit(*this, e->left);
  value right = std::visit(*this, e->right);

  switch (e->op.type) {
  case BANG_EQUAL:
    return left != right;
  case EQUAL_EQUAL:
    return left == right;
  case GREATER:
    return check_operands<double>(e->op, left, std::greater<double>{}, right);
  case GREATER_EQUAL:
    return check_operands<double>(e->op, left, std::greater_equal<double>{},
                                  right);
  case LESS:
    return check_operands<double>(e->op, left, std::less<double>{}, right);
  case LESS_EQUAL:
    return check_operands<double>(e->op, left, std::less_equal<double>{},
                                  right);
  case MINUS:
    return check_operands<double>(e->op, left, std::plus<double>{}, right);
  case PLUS:
    if (std::holds_alternative<double>(left) &&
        std::holds_alternative<double>(right)) {
      return std::get<double>(left) + std::get<double>(right);
    }

    if (std::holds_alternative<std::string>(left) &&
        std::holds_alternative<std::string>(right)) {
      return std::get<std::string>(left) + std::get<std::string>(right);
    }

    throw errors::runtime_error(e->op,
                                "operands must be two numbers or two strings");
  case SLASH:
    return check_operands<double>(e->op, left, std::divides<double>{}, right);
  case STAR:
    return check_operands<double>(e->op, left, std::multiplies<double>{},
                                  right);
  default:
    break;
  }

  __builtin_unreachable();
}

auto interpreter::operator()(box<conditional_expr> const& e) -> value {
  value cond = std::visit(*this, e->cond);
  
  // This implicitly converts any expression into a bool (may be unexpected)
  if (is_truthy(cond)) {
    return std::visit(*this, e->conseq);    
  } else {
    return std::visit(*this, e->alt);
  }
}

void interpret(expr ex) {
  try {
    value val = std::visit(interpreter{}, ex);
    fmt::print("{}\n", to_string(val));
  } catch (const errors::runtime_error& err) {
    errors::report_runtime_error(err);
  }
}

} // namespace lox
