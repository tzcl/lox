#include <lox/ast/ast_printer.hpp>
#include <lox/errors.hpp>
#include <lox/interpreter/interpreter.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <cmath>
#include <exception>
#include <memory>
#include <ranges>
#include <utility>

namespace lox {

interpreter::interpreter(std::ostream& output)
    : globals_{environment{}}, env_{globals_}, output_(output) {
  globals_.define("pi", 3.14);
  globals_.define("min", builtin{"min", 2, [](std::vector<value> args) {
                                   return values::less_equal(token{}, args[0],
                                                             args[1])
                                            ? args[0]
                                            : args[1];
                                 }});
}

struct break_exception final : public std::exception {
  [[nodiscard]] auto what() const noexcept -> char const* override {
    return "break: jumping out of loop";
  }
};

struct return_exception final : public std::exception {
  explicit return_exception(value result) : value(std::move(result)) {}

  value value;

  [[nodiscard]] auto what() const noexcept -> char const* override {
    return "return: jumping out of callable";
  }
};

static auto hash_expr(expr ex) -> std::string {
  // TODO: Fix me
  auto hash = std::visit(ast_printer{}, ex);
  return hash;
}

auto interpreter::lookup_var(token name, expr ex) -> value {
  auto hash = hash_expr(std::move(ex));
  if (locals.contains(hash)) {
    return env_.get(locals[hash], name);
  } else {
    return globals_.get(name);
  }
}

void interpreter::assign_var(token name, expr ex, value value) {
  auto hash = hash_expr(std::move(ex));
  if (locals.contains(hash)) env_.assign(locals[hash], name, value);
  else globals_.assign(name, value);
}

auto interpreter::operator()(literal_expr const& e) -> value {
  return values::to_value(e.literal);
}

auto interpreter::operator()(variable_expr const& e) -> value {
  return lookup_var(e.name, e);
}

auto interpreter::operator()(box<group_expr> const& e) -> value {
  return std::visit(*this, e->ex);
}

auto interpreter::operator()(box<assign_expr> const& e) -> value {
  value value = std::visit(*this, e->value);
  assign_var(e->name, e, value);
  return value;
}

auto interpreter::operator()(box<unary_expr> const& e) -> value {
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

auto interpreter::operator()(box<logical_expr> const& e) -> value {
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

auto interpreter::operator()(box<binary_expr> const& e) -> value {
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

auto interpreter::operator()(box<call_expr> const& e) -> value {
  value callee = std::visit(*this, e->callee);

  std::vector<value> args;
  for (auto const& arg : e->args) { args.push_back(std::visit(*this, arg)); }

  if (std::ssize(args) != values::arity(e->paren, callee)) {
    throw runtime_error(e->paren,
                        fmt::format("expected {} arguments but got {}",
                                    values::arity(e->paren, callee),
                                    std::ssize(args)));
  }

  return values::call(
      e->paren, callee, args,
      [this](callable callable, env_ptr const& env_ptr) -> value {
        return interpret(callable, env_ptr);
      });
}

auto interpreter::operator()(box<conditional_expr> const& e) -> value {
  value cond = std::visit(*this, e->cond);

  // This implicitly converts any expression into a bool (may be unexpected)
  if (values::is_truthy(cond)) {
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
  output_ << fmt::format("{}\n", values::to_string(value));
}

void interpreter::operator()(variable_stmt const& s) {
  value value;
  if (s.init) value = std::visit(*this, *s.init);

  env_.define(s.name.lexeme, value);
}

void interpreter::operator()(box<block_stmt> const& s) {
  environment prev = env_;
  env_             = environment{&prev};
  for (auto const& ss : s->stmts) { std::visit(*this, ss); }
  // TODO: This isn't exception-safe
  env_ = prev;
}

void interpreter::operator()(box<function_stmt> const& s) {
  // TODO: This probably dangles
  function fn{*s, std::make_shared<environment>(env_)};
  // TODO: Function can't refer to itself
  env_.define(s->name.lexeme, fn);
}

[[noreturn]] void interpreter::operator()(break_stmt const& /*s*/) {
  throw break_exception();
}

[[noreturn]] void interpreter::operator()(return_stmt const& s) {
  value value{};
  if (s.value) { value = std::visit(*this, *s.value); }

  throw return_exception{value};
}

void interpreter::operator()(box<if_stmt> const& s) {
  if (values::is_truthy(std::visit(*this, s->cond))) {
    std::visit(*this, s->then);
  } else if (s->alt) {
    std::visit(*this, *s->alt);
  }
}

void interpreter::operator()(box<while_stmt> const& s) {
  try {
    while (values::is_truthy(std::visit(*this, s->cond))) {
      std::visit(*this, s->body);
    }
  } catch (break_exception&) {
    // Break to end of loop
  }
}

void interpreter::interpret(std::vector<stmt> const& stmts) {
  try {
    for (auto const& s : stmts) {
      if (std::holds_alternative<expression_stmt>(s)) {
        auto  expr  = std::get<expression_stmt>(s);
        value value = std::visit(*this, expr.ex);
        output_ << fmt::format("{}\n", values::to_string(value));
      } else {
        std::visit(*this, s);
      }
    }
  } catch (runtime_error const& err) { errors::report_runtime_error(err); }
}

// Implements interpret_func
auto interpreter::interpret(callable callable, env_ptr const& env_ptr)
    -> value {
  environment prev = env_;
  try {
    environment env{env_ptr.get()};

    for (int i = 0; i < std::ssize(callable.params); ++i) {
      env.define(callable.params[i].lexeme, callable.args[i]);
    }

    env_ = env;
    interpret(callable.body);
  } catch (return_exception const& e) {
    env_ = prev;
    return e.value;
  }

  return {};
}

void interpreter::resolve(expr ex, int depth) {
  auto hash    = hash_expr(std::move(ex));
  locals[hash] = depth;
}

} // namespace lox
