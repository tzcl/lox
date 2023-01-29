#include <lox/errors.hpp>
#include <lox/parser.hpp>

#include <algorithm>
#include <exception>
#include <utility>

namespace lox {

using enum token_type;

// === Parse grammar ===
auto parser::parse() -> expr {
  // TODO: synchronise parser on err
  try {
    return expression();
  } catch (errors::parser_error const& err) {
    errors::report_parser_error(err);
    return {};
  }
}

auto parser::expression() -> expr {
  // TODO: add assignment etc. here
  return comma();
}

// TODO: Have to be careful with function parameters as this will cause
// f(1, 2) to be parsed as f((1, 2)).
auto parser::comma() -> expr {
  return left_assoc(&parser::conditional, {COMMA});
}

auto parser::conditional() -> expr {
  expr ex = equality();

  while (match({QUESTION})) {
    token hook   = prev();
    expr  conseq = expression(); // Allow for assignment in then branch

    consume(COLON, "expected alternate condition of ternary");

    token colon = prev();
    expr  alt   = conditional();

    ex = conditional_expr{ex, conseq, alt};
  }

  return ex;
}

auto parser::equality() -> expr {
  return left_assoc(&parser::comparison, {BANG_EQUAL, EQUAL_EQUAL});
}

auto parser::comparison() -> expr {
  return left_assoc(&parser::term, {GREATER, GREATER_EQUAL, LESS, LESS_EQUAL});
}

auto parser::term() -> expr {
  return left_assoc(&parser::factor, {MINUS, PLUS});
}

auto parser::factor() -> expr {
  return left_assoc(&parser::unary, {SLASH, STAR});
}

auto parser::unary() -> expr {
  if (match({BANG, MINUS})) {
    token op    = prev();
    expr  right = unary();
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

  // Check for binary operators missing their first expression
  missing_binary_op();

  throw errors::parser_error(peek(), "expected expression");
}

void parser::missing_binary_op() {
  if (match({COMMA})) {
    token op = prev();
    conditional();
    throw errors::parser_error(op, "missing left-hand operand");
  }

  if (match({BANG_EQUAL, EQUAL_EQUAL})) {
    token op = prev();
    equality();
    throw errors::parser_error(op, "missing left-hand operand");
  }

  if (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
    token op = prev();
    comparison();
    throw errors::parser_error(op, "missing left-hand operand");
  }

  if (match({PLUS})) {
    token op = prev();
    term();
    throw errors::parser_error(op, "missing left-hand operand");
  }

  if (match({SLASH, STAR})) {
    token op = prev();
    factor();
    throw errors::parser_error(op, "missing left-hand operand");
  }
}

template <typename R>
auto parser::left_assoc(R rule, std::initializer_list<token_type> types)
    -> expr {
  expr ex = (this->*rule)();

  while (match(types)) {
    token op    = prev();
    expr  right = (this->*rule)();
    ex          = binary_expr{ex, op, right};
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
  throw errors::parser_error(peek(), std::string(message));
}

void parser::synchronise() {
  next();

  while (!done()) {
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
    default:
      break;
    }

    next();
  }
}

} // namespace lox
