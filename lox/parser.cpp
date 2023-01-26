#include <lox/error.hpp>
#include <lox/parser.hpp>

#include <algorithm>
#include <exception>
#include <utility>

namespace lox {

// Sentinel exception for unwinding the parser.
//
// -Wweak-table isn't worth worrying too much about, only one vtable is left
// after the static link step and there isn't much compile time overhead.
// See Facebook's discussion: https://github.com/facebook/folly/issues/834
// However, LLVM recommend using an anchor to resolve this issue:
// https://llvm.org/docs/CodingStandards.html#provide-a-virtual-method-anchor-for-classes-in-headersclass
// parse_error : public std::exception {
class parse_error final : public std::exception {
public:
  [[nodiscard]] auto what() const noexcept -> const char* override {
    return "parser sentinel";
  }
  virtual void anchor();
};

void parse_error::anchor() {}

static auto raise_error(token token, std::string_view message) -> parse_error {
  error::parser_err(std::move(token), message);
  return {};
}

// TODO: pull in all token types
using enum lox::token_type;

auto parser::parse() -> expr {
  try {
    return expression();
  } catch (parse_error const&) { return {}; }
}

auto parser::expression() -> expr {
  // TODO: add assignment etc. here
  return equality();
}

auto parser::equality() -> expr {
  return left_assoc(std::bind_front(&parser::comparison, this),
                    {BANG_EQUAL, EQUAL_EQUAL});
}

auto parser::comparison() -> expr {
  return left_assoc(std::bind_front(&parser::term, this),
                    {GREATER, GREATER_EQUAL, LESS, LESS_EQUAL});
}

auto parser::term() -> expr {
  return left_assoc(std::bind_front(&parser::factor, this), {MINUS, PLUS});
}

auto parser::factor() -> expr {
  return left_assoc(std::bind_front(&parser::unary, this), {SLASH, STAR});
}

auto parser::unary() -> expr {
  if (match({BANG, MINUS})) {
    token op = prev();
    expr right = unary();
    return unary_expr{op, right};
  }

  return primary();
}

auto parser::primary() -> expr {
  if (match({FALSE})) return literal_expr{false};
  if (match({TRUE})) return literal_expr{true};
  if (match({NIL})) return literal_expr{};

  if (match({NUMBER, STRING})) { return literal_expr{prev().literal}; }

  if (match({LEFT_PAREN})) {
    expr ex = expression();
    consume(RIGHT_PAREN, "expected ')' after expression");
    return group_expr{ex};
  }

  throw raise_error(peek(), "expected expression");
}

template <typename R>
auto parser::left_assoc(R rule, std::initializer_list<token_type> types)
    -> expr {
  expr ex = rule();

  while (match(types)) {
    token op = prev();
    expr right = rule();
    ex = binary_expr{ex, op, right};
  }

  return ex;
}

auto parser::match(std::initializer_list<token_type> types) -> bool {
  for (token_type const& type : types) {
    if (check(type)) {
      next();
      return true;
    }
  }

  return false;
}

auto parser::consume(token_type type, std::string_view message) -> token {
  if (check(type)) return next();
  throw raise_error(peek(), message);
}

void parser::synchronise() {
  next();

  while (!done()) {
    // TODO: refactor to remove token_type?? I'm not fussed
    if (prev().type == SEMICOLON) return;

    // Skip over other statements
    switch (peek().type) {
    case CLASS:
    case FUN:
    case VAR:
    case FOR:
    case IF:
    case WHILE:
    case PRINT:
    case RETURN:
      return;
    }

    next();
  }
}

} // namespace lox
