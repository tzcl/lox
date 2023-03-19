#include <lox/errors.hpp>
#include <lox/interpreter/interpreter.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <cmath>
#include <exception>
#include <utility>

namespace lox {

struct break_exception final : public std::exception {
  [[nodiscard]] auto what() const noexcept -> const char* override {
    return "break: jumping out of loop";
  }
};

struct return_exception final : public std::exception {
  explicit return_exception(value result) : value(std::move(result)) {}

  value value;

  [[nodiscard]] auto what() const noexcept -> const char* override {
    return "return: jumping out of callable";
  }
};

auto interpreter::operator()(const literal_expr& e) -> value {
  return values::to_value(e.literal);
}

auto interpreter::operator()(const variable_expr& e) -> value {
  value value = env.get(e.name);
  return value;
}

auto interpreter::operator()(const box<group_expr>& e) -> value {
  return std::visit(*this, e->ex);
}

auto interpreter::operator()(const box<assign_expr>& e) -> value {
  value value = std::visit(*this, e->value);
  env.assign(e->name, value);
  return value;
}

auto interpreter::operator()(const box<unary_expr>& e) -> value {
  value right = std::visit(*this, e->right);

  switch (e->op.type) {
  case token_type::BANG:
    return !values::is_truthy(right);
  case token_type::MINUS:
    return values::negate(e->op, right);
  default:
    __builtin_unreachable();
  }
}

auto interpreter::operator()(const box<logical_expr>& e) -> value {
  value left = std::visit(*this, e->left);

  // Short-circuiting
  // Note that we return a 'truthy' value instead of a bool (lossy)
  if (e->op.type == token_type::OR) {
    if (values::is_truthy(left)) return left;
  } else {
    if (!values::is_truthy(left)) return left;
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
    return values::greater_than(e->op, left, right);
  case token_type::GREATER_EQUAL:
    return values::greater_equal(e->op, left, right);
  case token_type::LESS:
    return values::less_than(e->op, left, right);
  case token_type::LESS_EQUAL:
    return values::less_equal(e->op, left, right);
  case token_type::PLUS:
    return values::to_value(values::plus(e->op, left, right));
  case token_type::MINUS:
    return values::minus(e->op, left, right);
  case token_type::STAR:
    return values::to_value(values::multiply(e->op, left, right));
  case token_type::SLASH:
    return values::divide(e->op, left, right);
  default:
    throw runtime_error(e->op, "unhandled binary operator");
  }
}

auto interpreter::operator()(const box<call_expr>& e) -> value {
  value callee = std::visit(*this, e->callee);

  std::vector<value> args;
  for (const auto& arg : e->args) { args.push_back(std::visit(*this, arg)); }

  if (std::ssize(args) != values::arity(e->paren, callee)) {
    throw runtime_error(e->paren,
                        fmt::format("expected {} arguments but got {}",
                                    values::arity(e->paren, callee),
                                    std::ssize(args)));
  }

  return values::call(e->paren, callee, args, interpret);
}

auto interpreter::operator()(const box<conditional_expr>& e) -> value {
  value cond = std::visit(*this, e->cond);

  // This implicitly converts any expression into a bool (may be unexpected)
  if (values::is_truthy(cond)) {
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
  output << fmt::format("{}\n", values::to_string(value));
}

void interpreter::operator()(const variable_stmt& s) {
  value value;
  if (s.init) value = std::visit(*this, *s.init);

  env.define(s.name.lexeme, value);
}

void interpreter::operator()(const block_stmt& s) {
  interpreter interpreter{environment(&env), output}; // create new scope
  for (const auto& ss : s.stmts) { std::visit(interpreter, ss); }
}

void interpreter::operator()(const function_stmt& s) {
  function fn{s};
  env.define(s.name.lexeme, std::move(fn));
}

[[noreturn]] void interpreter::operator()(const break_stmt& /*s*/) {
  throw break_exception();
}

[[noreturn]] void interpreter::operator()(const return_stmt& s) {
  value value{};
  if (s.value) { value = std::visit(*this, *s.value); }

  throw return_exception{value};
}

void interpreter::operator()(const box<if_stmt>& s) {
  if (values::is_truthy(std::visit(*this, s->cond))) {
    std::visit(*this, s->then);
  } else if (s->alt) {
    std::visit(*this, *s->alt);
  }
}

void interpreter::operator()(const box<while_stmt>& s) {
  try {
    while (values::is_truthy(std::visit(*this, s->cond))) {
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
        interpreter.output << fmt::format("{}\n", values::to_string(value));
      } else {
        std::visit(interpreter, s);
      }
    }
  } catch (const runtime_error& err) { errors::report_runtime_error(err); }
}

auto interpret(callable callable) -> value {
  try {
    // TODO: Enable closures
    environment env{};
    for (int i = 0; i < std::ssize(callable.params); ++i) {
      env.define(callable.params[i].lexeme, callable.args[i]);
    }

    interpreter interpreter{env};
    interpret(interpreter, callable.body);
  } catch (const return_exception& e) { return e.value; }

  return {};
}

} // namespace lox
