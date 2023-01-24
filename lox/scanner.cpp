#include "lox/error.hpp"
#include <fmt/core.h>
#include <lox/scanner.hpp>

namespace lox {

auto scanner::tokens() -> std::vector<token> {
  while (!done()) {
    start_ = curr_;
    scan();
  }

  tokens_.push_back(token{token_type::EOF, "", line_});
  return tokens_;
}

void scanner::skip_block_comment() {
  // Naive implementation searches for the closing */ but that may overlap
  // a nested comment!
  // Idea: keep track of the levels of nesting
  for (int depth = 1; depth > 0;) {
    if (done()) {
      lox::error(line_, "Unterminated block comment");
      return;
    }

    if (match('/') && peek() == '*') {
      next();
      ++depth;
      continue;
    }

    if (match('*') && peek() == '/') {
      next();
      --depth;
      continue;
    }

    next();
  }
}

void scanner::scan() {
  char c = next();
  using enum lox::token_type; // C++20 addition!
  switch (c) {
  // clang-format off
  case '(': add_token(LEFT_PAREN); break;
  case ')': add_token(RIGHT_PAREN); break;
  case '{': add_token(LEFT_BRACE); break;
  case '}': add_token(RIGHT_BRACE); break;
  case ',': add_token(COMMA); break;
  case '.': add_token(DOT); break;
  case '-': add_token(MINUS); break;
  case '+': add_token(PLUS); break;
  case ';': add_token(SEMICOLON); break;
  case '*': add_token(STAR); break;

  case '"': string(); break;
    // clang-format on

  case '!':
    add_token(match('=') ? BANG_EQUAL : BANG);
    break;
  case '=':
    add_token(match('=') ? EQUAL_EQUAL : EQUAL);
    break;
  case '<':
    add_token(match('=') ? LESS_EQUAL : LESS);
    break;
  case '>':
    add_token(match('=') ? GREATER_EQUAL : GREATER);
    break;
  case '/':
    if (match('/')) {
      // A comment goes until the end of the line
      while (peek() != '\n' && !done()) next();
    } else if (match('*')) {
      // Find closing */
      skip_block_comment();
    } else {
      add_token(SLASH);
    }
    break;

  case ' ':
  case '\r':
  case '\t':
    // Ignore whitespace
    break;
  case '\n':
    ++line_;
    break;

  case 'o':
    if (match('r')) { add_token(OR); }
    break;

  default:
    // TODO: report error here
    if (is_digit(c)) {
      number();
    } else if (is_alpha(c)) {
      identifier();
    } else {
      lox::error(line_, fmt::format("Unexpected character: {}", c));
    }
    break;
  }
}

void scanner::string() {
  while (peek() != '"' && !done()) {
    if (peek() == '\n') ++line_;
    next();
  }

  if (done()) {
    // TODO: err here
    lox::error(line_,
               fmt::format("Unterminated string: {}", substr(start_, curr_)));
    return;
  }

  // The closing "
  next();

  // Trim the surrounding quotes
  add_token(token_type::STRING, substr(start_ + 1, curr_ - 1));
}

void scanner::number() {
  while (is_digit(peek())) next();

  // Look for a fractional part
  if (peek() == '.' && is_digit(peek_next())) {
    // Consume the "."
    next();

    while (is_digit(peek())) next();
  }

  double num = std::stod(substr(start_, curr_));
  add_token(token_type::NUMBER, num);
}

void scanner::identifier() {
  while (is_alphanumeric(peek())) next();

  token_type type = keywords.at(substr(start_, curr_));
  add_token(type);
}

} // namespace lox
