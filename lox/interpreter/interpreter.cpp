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
    : globals_{environment{}}, env{std::make_shared<environment>(&globals_)},
      output_(output) {
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

struct visitor {
  env_ptr       env;
  std::ostream& output;

  auto operator()(literal_expr const& e) -> value;
  auto operator()(variable_expr const& e) -> value;
  auto operator()(box<group_expr> const& e) -> value;
  auto operator()(box<assign_expr> const& e) -> value;
  auto operator()(box<unary_expr> const& e) -> value;
  auto operator()(box<logical_expr> const& e) -> value;
  auto operator()(box<binary_expr> const& e) -> value;
  auto operator()(box<call_expr> const& e) -> value;
  auto operator()(box<conditional_expr> const& e) -> value;

  // TODO: This feels hacky
  auto interpret(callable callable, env_ptr const& env_ptr) -> value;

  void operator()(expression_stmt const& s);
  void operator()(print_stmt const& s);
  void operator()(variable_stmt const& s);
  void operator()(box<block_stmt> const& s);
  void operator()(box<function_stmt> const& s);
  void operator()(box<if_stmt> const& s);
  void operator()(box<while_stmt> const& s);

  [[noreturn]] void operator()(break_stmt const& s);
  [[noreturn]] void operator()(return_stmt const& s);
};

auto visitor::operator()(literal_expr const& e) -> value {
  return values::to_value(e.literal);
}

auto visitor::operator()(variable_expr const& e) -> value {
  return env->get(e.name);
}

auto visitor::operator()(box<group_expr> const& e) -> value {
  return std::visit(*this, e->ex);
}

auto visitor::operator()(box<assign_expr> const& e) -> value {
  value value = std::visit(*this, e->value);
  env->assign(e->name, value);
  return value;
}

auto visitor::operator()(box<unary_expr> const& e) -> value {
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

auto visitor::operator()(box<logical_expr> const& e) -> value {
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

auto visitor::operator()(box<binary_expr> const& e) -> value {
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

auto visitor::operator()(box<call_expr> const& e) -> value {
  value callee = std::visit(*this, e->callee);

  std::vector<value> args;
  for (auto const& arg : e->args) { args.push_back(std::visit(*this, arg)); }

  if (std::ssize(args) != values::arity(e->paren, callee)) {
    throw runtime_error(e->paren,
                        fmt::format("expected {} arguments but got {}",
                                    values::arity(e->paren, callee),
                                    std::ssize(args)));
  }

  // TODO: Fix me up
  return values::call(
      e->paren, callee, args,
      [this](callable callable, env_ptr const& env_ptr) -> value {
        return interpret(callable, env_ptr);
      });
}

auto visitor::operator()(box<conditional_expr> const& e) -> value {
  value cond = std::visit(*this, e->cond);

  // This implicitly converts any expression into a bool (may be unexpected)
  if (values::is_truthy(cond)) {
    return std::visit(*this, e->then);
  } else {
    return std::visit(*this, e->alt);
  }
}

void visitor::operator()(expression_stmt const& s) { std::visit(*this, s.ex); }

void visitor::operator()(print_stmt const& s) {
  value value = std::visit(*this, s.ex);
  // TODO: How to get output to visitor?
  output << fmt::format("{}\n", values::to_string(value));
}

void visitor::operator()(variable_stmt const& s) {
  value value;
  if (s.init) value = std::visit(*this, *s.init);

  env->define(s.name.lexeme, value);
}

void visitor::operator()(box<block_stmt> const& s) {
  visitor visitor{std::make_shared<environment>(env.get()), output};
  for (auto const& ss : s->stmts) { std::visit(visitor, ss); }
}

void visitor::operator()(box<function_stmt> const& s) {
  function fn{*s, env};
  env->define(s->name.lexeme, fn);
}

[[noreturn]] void visitor::operator()(break_stmt const& /*s*/) {
  throw break_exception();
}

[[noreturn]] void visitor::operator()(return_stmt const& s) {
  value value{};
  if (s.value) { value = std::visit(*this, *s.value); }

  throw return_exception{value};
}

void visitor::operator()(box<if_stmt> const& s) {
  if (values::is_truthy(std::visit(*this, s->cond))) {
    std::visit(*this, s->then);
  } else if (s->alt) {
    std::visit(*this, *s->alt);
  }
}

void visitor::operator()(box<while_stmt> const& s) {
  try {
    while (values::is_truthy(std::visit(*this, s->cond))) {
      std::visit(*this, s->body);
    }
  } catch (break_exception&) {
    // Break to end of loop
  }
}

static void interpret_stmts(visitor visitor, std::vector<stmt> const& stmts) {
  try {
    for (auto const& s : stmts) {
      if (std::holds_alternative<expression_stmt>(s)) {
        auto  expr  = std::get<expression_stmt>(s);
        value value = std::visit(visitor, expr.ex);
        visitor.output << fmt::format("{}\n", values::to_string(value));
      } else {
        std::visit(visitor, s);
      }
    }
  } catch (runtime_error const& err) { errors::report_runtime_error(err); }
}

void interpreter::interpret(std::vector<stmt> const& stmts) {
  visitor visitor{env, output_};
  interpret_stmts(visitor, stmts);
}

// Implements interpret_func
auto visitor::interpret(callable callable, env_ptr const& env_ptr) -> value {
  try {
    auto scope = std::make_shared<environment>(env_ptr.get());

    for (int i = 0; i < std::ssize(callable.params); ++i) {
      scope->define(callable.params[i].lexeme, callable.args[i]);
    }

    visitor visitor{scope, output};
    interpret_stmts(visitor, callable.body);
  } catch (return_exception const& e) { return e.value; }

  return {};
}

} // namespace lox
