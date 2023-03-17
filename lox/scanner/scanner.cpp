#include <lox/errors.hpp>
#include <lox/scanner/scanner.hpp>

#include <fmt/core.h>

namespace lox {

static auto get_token_type(std::string_view ident) -> token_type {
  if (ident == "and") return token_type::AND;
  if (ident == "class") return token_type::CLASS;
  if (ident == "else") return token_type::ELSE;
  if (ident == "false") return token_type::FALSE;
  if (ident == "for") return token_type::FOR;
  if (ident == "fun") return token_type::FUN;
  if (ident == "if") return token_type::IF;
  if (ident == "nil") return token_type::NIL;
  if (ident == "or") return token_type::OR;
  if (ident == "print") return token_type::PRINT;
  if (ident == "return") return token_type::RETURN;
  if (ident == "super") return token_type::SUPER;
  if (ident == "this") return token_type::THIS;
  if (ident == "true") return token_type::TRUE;
  if (ident == "var") return token_type::VAR;
  if (ident == "while") return token_type::WHILE;
  if (ident == "break") return token_type::BREAK;

  return token_type::IDENTIFIER;
}

// clang-format off
static auto is_digit(char c) -> bool { return c >= '0' && c <= '9'; }
static auto is_alpha(char c) -> bool { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
static auto is_alphanumeric(char c) -> bool { return is_alpha(c) || is_digit(c); }
// clang-format on

auto scanner::scan() -> std::vector<token> {
  while (!done()) {
    start_ = curr_;
    scan_next();
  }

  tokens_.push_back(token{token_type::EOF, "", line_});
  return tokens_;
}

void scanner::scan_next() {
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
  case '?': add_token(QUESTION); break;
  case ':': add_token(COLON); break;

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
      block_comment();
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
    if (is_digit(c)) {
      number();
    } else if (is_alpha(c)) {
      identifier();
    } else {
      error::report(line_, fmt::format("unexpected character: {}", c));
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
    error::report(
        line_, fmt::format("unterminated string: {}", substr(start_, curr_)));
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

  token_type type = get_token_type(substr(start_, curr_));
  add_token(type);
}

void scanner::block_comment() {
  // A naive implementation searches for the closing */ but that may overlap
  // a nested comment!
  // Idea: keep track of the levels of nesting.
  for (int depth = 1; depth > 0;) {
    if (done()) {
      error::report(line_, "unterminated block comment");
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

    if (match('\n')) {
      line_++;
      continue;
    }

    next();
  }
}

} // namespace lox
