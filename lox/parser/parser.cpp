#include <lox/errors.hpp>
#include <lox/parser/parser.hpp>

#include <algorithm>
#include <exception>
#include <optional>
#include <utility>

namespace lox {

using enum token_type;

// === Parse grammar ===
auto parser::parse() -> std::vector<stmt> {
  std::vector<stmt> stmts;
  while (!done()) { stmts.push_back(declaration()); }

  return stmts;
}

auto parser::declaration() -> stmt {
  try {
    if (match({VAR})) return var_declaration();
    return statement();
  } catch (parser_error& err) {
    errors::report_parser_error(err);
    synchronise();
    return expression_stmt{};
  }
}

auto parser::var_declaration() -> stmt {
  token name = consume(IDENTIFIER, "Expect variable name");

  expr init;
  if (match({EQUAL})) { init = expression(); }

  consume(SEMICOLON, "Expect ';' after variable declaration");
  return variable_stmt{name, init};
}

auto parser::statement() -> stmt {
  if (match({IF})) return if_statement();
  if (match({PRINT})) return print_statement();
  if (match({FOR})) return for_statement();
  if (match({WHILE})) return while_statement();
  if (match({LEFT_BRACE})) return block_stmt{block_statement()};
  if (match({BREAK})) return break_statement();

  return expression_statement();
}

auto parser::if_statement() -> stmt {
  consume(LEFT_PAREN, "expected '(' after 'if'");
  expr cond = expression();
  consume(RIGHT_PAREN, "expected ')' after if condition");

  stmt then = statement();
  if (match({ELSE})) {
    stmt alt = statement();
    return if_stmt{cond, then, alt};
  }

  return if_stmt{cond, then};
}

auto parser::print_statement() -> stmt {
  expr value = expression();
  consume(SEMICOLON, "Expect ';' after value");
  return print_stmt{value};
}

auto parser::for_statement() -> stmt {
  consume(LEFT_PAREN, "Expect '(' after 'for'");

  std::optional<stmt> init;
  if (!match({SEMICOLON})) {
    if (match({VAR})) init = var_declaration();
    else init = expression_statement();
  }

  std::optional<expr> cond;
  if (!check(SEMICOLON)) cond = expression();
  consume(SEMICOLON, "Expect ';' after loop condition");

  std::optional<expr> after;
  if (!check(RIGHT_PAREN)) after = expression();
  consume(RIGHT_PAREN, "Expect ')' after for clauses");

  try {
    ++loop_depth_;

    stmt body = statement();

    // Rewrite for loop into equivalent while loop
    if (after) { body = block_stmt{{body, expression_stmt{*after}}}; }

    if (!cond) cond = literal_expr{true};
    body = while_stmt{*cond, body};

    if (init) body = block_stmt{{*init, body}};

    return body;
  } catch (std::exception& e) {
    // Catch and rethrow to ensure loop depth gets decremented
    --loop_depth_;
    throw e;
  }
}

auto parser::while_statement() -> stmt {
  consume(LEFT_PAREN, "Expect '(' after 'while'");
  expr cond = expression();
  consume(RIGHT_PAREN, "Expect ')' after condition");

  try {
    ++loop_depth_;
    stmt body = statement();

    return while_stmt{cond, body};
  } catch (std::exception& e) {
    --loop_depth_;
    throw e;
  }
}

auto parser::block_statement() -> std::vector<stmt> {
  std::vector<stmt> stmts;
  while (!check(RIGHT_BRACE) && !done()) stmts.push_back(declaration());
  consume(RIGHT_BRACE, "Expect '}' after block");

  return stmts;
}

auto parser::break_statement() -> stmt {
  if (loop_depth_ == 0) throw parser_error(prev(), "'break' outside of loop");
  consume(SEMICOLON, "Expect ';' after 'break'");
  return break_stmt{loop_depth_};
}

auto parser::expression_statement() -> stmt {
  expr ex = expression();
  consume(SEMICOLON, "Expect ';' after expression");
  return expression_stmt{ex};
}

auto parser::expression() -> expr { return assignment(); }

auto parser::assignment() -> expr {
  expr lhs = logic_or();

  if (match({EQUAL})) {
    token equals = prev();
    expr  rhs    = assignment(); // Recurse here (right-associative)

    if (std::holds_alternative<variable_expr>(lhs)) {
      auto  var_ex = std::get<variable_expr>(lhs);
      token name   = var_ex.name;
      return assign_expr{name, rhs};
    }

    // Report but don't throw an error because we don't want to synchronise
    errors::report(equals.line, "Invalid assignment target");
  }

  return lhs;
}

auto parser::logic_or() -> expr {
  return left_assoc<logical_expr>(&parser::logic_and, {OR});
}

auto parser::logic_and() -> expr {
  return left_assoc<logical_expr>(&parser::comma, {AND});
}

// TODO: Have to be careful with function parameters as this will cause
// f(1, 2) to be parsed as f((1, 2)).
auto parser::comma() -> expr {
  return left_assoc<binary_expr>(&parser::conditional, {COMMA});
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
  return left_assoc<binary_expr>(&parser::comparison,
                                 {BANG_EQUAL, EQUAL_EQUAL});
}

auto parser::comparison() -> expr {
  return left_assoc<binary_expr>(&parser::term,
                                 {GREATER, GREATER_EQUAL, LESS, LESS_EQUAL});
}

auto parser::term() -> expr {
  return left_assoc<binary_expr>(&parser::factor, {MINUS, PLUS});
}

auto parser::factor() -> expr {
  return left_assoc<binary_expr>(&parser::unary, {SLASH, STAR});
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
  if (match({NIL, FALSE, TRUE, NUMBER, STRING})) {
    return literal_expr{prev().literal};
  }

  if (match({IDENTIFIER})) return variable_expr{prev()};

  if (match({LEFT_PAREN})) {
    expr ex = expression();
    consume(RIGHT_PAREN, "expected ')' after expression");
    return group_expr{ex};
  }

  // Check for binary operators missing their first expression
  missing_binary_op();

  throw parser_error(peek(), "expected expression");
}

void parser::missing_binary_op() {
  if (match({COMMA})) {
    token op = prev();
    conditional();
    throw parser_error(op, "missing left-hand operand");
  }

  if (match({BANG_EQUAL, EQUAL_EQUAL})) {
    token op = prev();
    equality();
    throw parser_error(op, "missing left-hand operand");
  }

  if (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
    token op = prev();
    comparison();
    throw parser_error(op, "missing left-hand operand");
  }

  if (match({PLUS})) {
    token op = prev();
    term();
    throw parser_error(op, "missing left-hand operand");
  }

  if (match({SLASH, STAR})) {
    token op = prev();
    factor();
    throw parser_error(op, "missing left-hand operand");
  }
}

template <typename A, typename R>
auto parser::left_assoc(R rule, std::initializer_list<token_type> types)
    -> expr {
  expr ex = (this->*rule)();

  while (match(types)) {
    token op    = prev();
    expr  right = (this->*rule)();
    ex          = A{ex, op, right};
  }

  return ex;
}

auto parser::match(std::initializer_list<token_type> types) -> bool {
  for (token_type const& type : types) { // NOLINT(readability-use-anyofallof)
    if (check(type)) {
      next();
      return true;
    }
  }

  return false;
}

auto parser::consume(token_type type, std::string_view message) -> token {
  if (check(type)) return next();
  throw parser_error(peek(), std::string(message));
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
