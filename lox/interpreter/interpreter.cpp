#include <lox/errors.hpp>
#include <lox/interpreter/interpreter.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cmath>
#include <functional>
#include <utility>

namespace lox {

struct break_exception final : public std::exception {
  [[nodiscard]] auto what() const noexcept -> const char* override {
    return "break: jumping out of loop";
  }
};

auto interpreter::operator()(const literal_expr& e) -> value {
  return to_value(e.literal);
}

auto interpreter::operator()(const variable_expr& e) -> value {
  // value value = env.get(e.name);
  // return value;
  return "variable_expr";
}

auto interpreter::operator()(const box<group_expr>& e) -> value {
  return std::visit(*this, e->ex);
}

auto interpreter::operator()(const box<assign_expr>& e) -> value {
  // value value = std::visit(*this, e->value);
  // env.assign(e->name, value);
  // return value;
  return "assign_expr";
}

auto interpreter::operator()(const box<unary_expr>& e) -> value {
  value right = std::visit(*this, e->right);

  switch (e->op.type) {
  case token_type::BANG:
    return !is_truthy(right);
  case token_type::MINUS:
    return negate(e->op, right);
  default:
    break;
  }

  __builtin_unreachable();
}

auto interpreter::operator()(const box<logical_expr>& e) -> value {
  value left = std::visit(*this, e->left);

  // Short-circuiting
  // Note that we return a 'truthy' value instead of a bool (lossy)
  if (e->op.type == token_type::OR) {
    if (is_truthy(left)) return left;
  } else {
    if (!is_truthy(left)) return left;
  }

  return std::visit(*this, e->right);
}

auto interpreter::operator()(const box<binary_expr>& e) -> value {
  // Note, we evaluate the LHS before the RHS.
  // Also, we evaluate both operands before checking their types are valid.
  value left  = std::visit(*this, e->left);
  value right = std::visit(*this, e->right);

  switch (e->op.type) {
  case token_type::COMMA:
    return right;
  case token_type::BANG_EQUAL:
    return left != right;
  case token_type::EQUAL_EQUAL:
    return left == right;
  case token_type::GREATER:
    return greater_than(e->op, left, right);
  case token_type::GREATER_EQUAL:
    return greater_equal(e->op, left, right);
  case token_type::LESS:
    return less_than(e->op, left, right);
  case token_type::LESS_EQUAL:
    return less_equal(e->op, left, right);
  case token_type::PLUS:
    return to_value(plus(e->op, left, right));
  case token_type::MINUS:
    return minus(e->op, left, right);
  case token_type::STAR:
    return to_value(multiply(e->op, left, right));
  case token_type::SLASH:
    return divide(e->op, left, right);
  default:
    throw runtime_error(e->op, "unhandled binary operator");
  }
}

auto interpreter::operator()(const box<conditional_expr>& e) -> value {
  value cond = std::visit(*this, e->cond);

  // This implicitly converts any expression into a bool (may be unexpected)
  if (is_truthy(cond)) {
    return std::visit(*this, e->then);
  } else {
    return std::visit(*this, e->alt);
  }
}

void interpreter::operator()(const expression_stmt& s) {
  std::visit(*this, s.ex);
}

void interpreter::operator()(const print_stmt& s) {
  value value = std::visit(*this, s.ex);
  output << fmt::format("{}\n", to_string(value));
}

void interpreter::operator()(const variable_stmt& s) {
  value value;
  if (s.init) value = std::visit(*this, *s.init);

  // env.set(s.name.lexeme, value);
}

void interpreter::operator()(const block_stmt& s) {
  // interpreter interpreter{environment(&env), output}; // create new scope
  for (const auto& ss : s.stmts) { std::visit(*this, ss); }
}

[[noreturn]] void interpreter::operator()(const break_stmt& /*s*/) {
  throw break_exception();
}

void interpreter::operator()(const box<if_stmt>& s) {
  if (is_truthy(std::visit(*this, s->cond))) {
    std::visit(*this, s->then);
  } else if (s->alt) {
    std::visit(*this, *s->alt);
  }
}

void interpreter::operator()(const box<while_stmt>& s) {
  try {
    while (is_truthy(std::visit(*this, s->cond))) {
      std::visit(*this, s->body);
    }
  } catch (break_exception&) {
    // Break to end of loop
  }
}

// Use a reference because we want an in-out parameter.
void interpret(interpreter& interpreter, const std::vector<stmt>& stmts) {
  try {
    for (const auto& s : stmts) {
      if (std::holds_alternative<expression_stmt>(s)) {
        auto  expr  = std::get<expression_stmt>(s);
        value value = std::visit(interpreter, expr.ex);
        interpreter.output << fmt::format("{}\n", to_string(value));
      } else {
        std::visit(interpreter, s);
      }
    }
  } catch (const runtime_error& err) { errors::report_runtime_error(err); }
}

} // namespace lox
