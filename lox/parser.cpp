#include <lox/parser.hpp>

#include <algorithm>

namespace lox {

auto parser::expression() -> expr {
  // TODO: add assignment etc. here
  return equality();
}

auto parser::equality() -> expr {
  expr ex = comparison();

  using enum lox::token_type;
  while (match({BANG_EQUAL, EQUAL_EQUAL})) {
    token op = prev();
    expr right = comparison();
    ex = binary_expr{ex, op, right};
  }

  return ex;
}

auto parser::comparison() -> expr {
  expr ex = term();

  using enum lox::token_type;
  while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
    token op = prev();
    expr right = term();
    ex = binary_expr{ex, op, right};
  }

  return ex;
}

auto parser::term() -> expr {
  expr ex = factor();

  using enum lox::token_type;
  while (match({MINUS, PLUS})) {
    token op = prev();
    expr right = factor();
    ex = binary_expr{ex, op, right};
  }

  return ex;
}

auto parser::term() -> expr {
  expr ex = unary();

  using enum lox::token_type;
  while (match({MINUS, PLUS})) {
    token op = prev();
    expr right = unary();
    ex = binary_expr{ex, op, right};
  }

  return ex;
}

auto parser::unary() -> expr {
  using enum lox::token_type;
  if (match({MINUS, PLUS})) {
    token op = prev();
    expr right = unary();
    return unary_expr{op, right};
  }

  return primary();
}

auto parser::primary() -> expr {
  using enum lox::token_type;
  if (match({FALSE})) return literal_expr{false};
  if (match({TRUE})) return literal_expr{true};
  if (match({NIL})) return literal_expr{};

  if (match({NUMBER, STRING})) return literal_expr{prev().literal};

  if (match({LEFT_PAREN})) {
    expr ex = expression();
    consume(RIGHT_PAREN, "Expect ')' after expression.");
    return grouping_expr{ex};
  }
}

// TODO: Try implementing this. Seems like I might have to use templates?
// https://www.youtube.com/watch?v=Rbl3h0RJuuY
auto parser::left_assoc(auto&& rule, std::initializer_list<token_type> types)
    -> expr {
  expr ex = rule();

  using enum lox::token_type;
  while (match(types)) {
    token op = prev();
    expr right = rule();
    ex = binary_expr{ex, op, right};
  }

  return ex;
}

auto parser::match(std::initializer_list<token_type> types) -> bool {
  // I didn't know about all_of/any_of/none_of, but they're more explicit
  // than writing a for loop (although I'm not sure what's more readable).
  return std::ranges::any_of(types,
                             [this](token_type type) { return check(type); });
}

} // namespace lox
