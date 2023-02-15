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
static auto get_op(token op, value val) -> T {
  try {
    return std::get<T>(val);
  } catch (std::bad_variant_access&) {
    throw errors::runtime_error(std::move(op), "operand must be a number");
  }
}

template <typename T>
struct binary_ops {
  T left, right;
};

template <typename T>
static auto check_binary_ops(value left, value right) -> bool {
  return std::holds_alternative<T>(left) && std::holds_alternative<T>(right);
}

template <typename T>
static auto get_binary_ops(token token, value left, value right)
    -> binary_ops<T> {
  try {
    return {std::get<T>(left), std::get<T>(right)};
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
    return -get_op<double>(e->op, right);
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
    if (check_binary_ops<double>(left, right)) {
      auto ops = get_binary_ops<double>(e->op, left, right);
      return ops.left > ops.right;
    }
    if (check_binary_ops<std::string>(left, right)) {
      auto ops = get_binary_ops<std::string>(e->op, left, right);
      return ops.left > ops.right;
    }

    throw errors::runtime_error(e->op,
                                "operands must be two numbers or two strings");
  case GREATER_EQUAL:
    if (check_binary_ops<double>(left, right)) {
      auto ops = get_binary_ops<double>(e->op, left, right);
      return ops.left >= ops.right;
    }
    if (check_binary_ops<std::string>(left, right)) {
      auto ops = get_binary_ops<std::string>(e->op, left, right);
      return ops.left >= ops.right;
    }

    throw errors::runtime_error(e->op,
                                "operands must be two numbers or two strings");

  case LESS:
    if (check_binary_ops<double>(left, right)) {
      auto ops = get_binary_ops<double>(e->op, left, right);
      return ops.left < ops.right;
    }
    if (check_binary_ops<std::string>(left, right)) {
      auto ops = get_binary_ops<std::string>(e->op, left, right);
      return ops.left < ops.right;
    }

    throw errors::runtime_error(e->op,
                                "operands must be two numbers or two strings");
  case LESS_EQUAL:
    if (check_binary_ops<double>(left, right)) {
      auto ops = get_binary_ops<double>(e->op, left, right);
      return ops.left <= ops.right;
    }
    if (check_binary_ops<std::string>(left, right)) {
      auto ops = get_binary_ops<std::string>(e->op, left, right);
      return ops.left <= ops.right;
    }

    throw errors::runtime_error(e->op,
                                "operands must be two numbers or two strings");
  case MINUS: {
    auto ops = get_binary_ops<double>(e->op, left, right);
    return ops.left - ops.right;
  }
  case PLUS:
    if (check_binary_ops<double>(left, right)) {
      auto ops = get_binary_ops<double>(e->op, left, right);
      return ops.left + ops.right;
    }
    if (check_binary_ops<std::string>(left, right)) {
      auto ops = get_binary_ops<std::string>(e->op, left, right);
      return ops.left + ops.right;
    }
    if (std::holds_alternative<std::string>(left)) {
      auto lval = std::get<std::string>(left);
      auto rval = to_string(right);
      return lval + rval;
    }
    if (std::holds_alternative<std::string>(right)) {
      auto lval = to_string(left);
      auto rval = std::get<std::string>(right);
      return lval + rval;
    }

    throw errors::runtime_error(e->op, "operands are incompatible");
  case SLASH: {
    auto ops = get_binary_ops<double>(e->op, left, right);
    if (std::abs(ops.right) < 1e-10)
      throw errors::runtime_error(e->op, "division by zero");
    return ops.left / ops.right;
  }
  case STAR: {
    auto ops = get_binary_ops<double>(e->op, left, right);
    return ops.left * ops.right;
  }
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
    fmt::print("{}\n", print_value(val));
  } catch (const errors::runtime_error& err) {
    errors::report_runtime_error(err);
  }
}

} // namespace lox
