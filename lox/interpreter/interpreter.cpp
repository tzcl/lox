#include <lox/errors.hpp>
#include <lox/interpreter/interpreter.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cmath>
#include <functional>
#include <utility>

namespace lox {

const static double EPSILON = 1e-10;

using enum token_type;

template <typename T>
static auto get_op(token op, value value) -> T {
  try {
    return std::get<T>(std::get<literal>(value));
  } catch (std::bad_variant_access&) {
    throw errors::runtime_error(std::move(op), "operand must be a number");
  }
}

template <typename T>
struct binary_ops {
  T left, right;
};

template <typename T>
static auto check_binary_ops(literal left, literal right) -> bool {
  return std::holds_alternative<T>(left) && std::holds_alternative<T>(right);
}

template <typename T>
static auto get_binary_ops(token token, literal left, literal right)
    -> binary_ops<T> {
  try {
    return {std::get<T>(left), std::get<T>(right)};
  } catch (std::bad_variant_access&) {
    throw errors::runtime_error(std::move(token), "operands must be numbers");
  }
}

struct break_exception final : public std::exception {
  [[nodiscard]] auto what() const noexcept -> const char* override {
    return "break: jumping out of loop";
  }

private:
  virtual void anchor();
};

void break_exception::anchor() {}

auto interpreter::operator()(literal_expr const& e) -> value { return e.value; }

auto interpreter::operator()(variable_expr const& e) -> value {
  value value = env.get(e.name);

  try {
    literal lit = std::get<literal>(value);
    if (lit.index() == 0)
      throw errors::runtime_error(e.name, "variable is uninitialised");
  } catch (std::bad_variant_access&) {
    throw errors::runtime_error(e.name, "variable is uninitialised");
  }

  return value;
}

auto interpreter::operator()(box<group_expr> const& e) -> value {
  return std::visit(*this, e->ex);
}

auto interpreter::operator()(box<assign_expr> const& e) -> value {
  value value = std::visit(*this, e->value);
  env.assign(e->name, value);
  return value;
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

auto interpreter::operator()(box<logical_expr> const& e) -> value {
  value left = std::visit(*this, e->left);

  // Short-circuiting
  // Note that we return a 'truthy' value instead of a bool (lossy)
  if (e->op.type == OR) {
    if (is_truthy(left)) return left;
  } else {
    if (!is_truthy(left)) return left;
  }

  return std::visit(*this, e->right);
}

auto interpreter::operator()(box<binary_expr> const& e) -> value {
  // Here is an important semantic choice: we evaluate the LHS before the RHS.
  // Also, we evaluate both operands before checking their types are valid.
  literal left  = get_literal(e->op, std::visit(*this, e->left));
  literal right = get_literal(e->op, std::visit(*this, e->right));

  switch (e->op.type) {
  case COMMA:
    return right;
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
    if (std::abs(ops.right) < EPSILON)
      throw errors::runtime_error(e->op, "division by zero");
    return ops.left / ops.right;
  }
  case STAR: {
    auto ops = get_binary_ops<double>(e->op, left, right);
    return ops.left * ops.right;
  }
  default:
    throw errors::runtime_error(e->op, "Unhandled binary operator");
  }
}

auto interpreter::operator()(box<conditional_expr> const& e) -> value {
  value cond = std::visit(*this, e->cond);

  // This implicitly converts any expression into a bool (may be unexpected)
  if (is_truthy(cond)) {
    return std::visit(*this, e->then);
  } else {
    return std::visit(*this, e->alt);
  }
}

void interpreter::operator()(expression_stmt const& s) {
  std::visit(*this, s.ex);
}

void interpreter::operator()(print_stmt const& s) {
  value value = std::visit(*this, s.ex);
  output << fmt::format("{}\n", to_string(value));
}

void interpreter::operator()(variable_stmt const& s) {
  value value;
  if (s.init) value = std::visit(*this, *s.init);

  env.set(s.name.lexeme, value);
}

void interpreter::operator()(block_stmt const& s) {
  interpreter interpreter{environment(&env), output}; // create new scope
  for (const auto& ss : s.stmts) { std::visit(interpreter, ss); }
}

[[noreturn]] void interpreter::operator()(break_stmt const& /*s*/) {
  throw break_exception();
}

void interpreter::operator()(box<if_stmt> const& s) {
  if (is_truthy(std::visit(*this, s->cond))) {
    std::visit(*this, s->then);
  } else if (s->alt) {
    std::visit(*this, *s->alt);
  }
}

void interpreter::operator()(box<while_stmt> const& s) {
  try {
    while (is_truthy(std::visit(*this, s->cond))) {
      std::visit(*this, s->body);
    }
  } catch (break_exception&) {
    // Break to end of loop
  }
}

// Use a reference because we want an in-out parameter.
void interpret(interpreter& interpreter, std::vector<stmt> const& stmts) {
  try {
    for (auto const& s : stmts) {
      if (std::holds_alternative<expression_stmt>(s)) {
        auto  expr  = std::get<expression_stmt>(s);
        value value = std::visit(interpreter, expr.ex);
        interpreter.output << fmt::format("{}\n", to_string(value));
      } else {
        std::visit(interpreter, s);
      }
    }
  } catch (const errors::runtime_error& err) {
    errors::report_runtime_error(err);
  }
}

} // namespace lox
