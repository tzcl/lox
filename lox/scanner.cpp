#include <lox/scanner.hpp>

namespace lox {
auto scanner::scan() -> std::vector<token> {
  while (!done()) {
    start_ = curr_;
    scan_token();
  }

  tokens_.emplace_back(token_type::EOF, "", line_);
  return tokens_;
}

void scanner::scan_token() {
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
      // TODO: Implement C-style /* ... */ block comments. Allow nesting.
      while (peek() != '\n' && !done())
        next();
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
    if (match('r')) {
      add_token(OR);
    }
    break;

  default:
    // TODO: report error here
    if (is_digit(c)) {
      number();
    } else if (is_alpha(c)) {
      identifier();
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
  }

  // The closing "
  next();

  // Trim the surrounding quotes
  add_token(token_type::STRING,
            std::string(source_.substr(start_ + 1, size_() - 1)));
}

void scanner::number() {
  while (is_digit(peek()))
    next();

  // Look for a fractional part
  if (peek() == '.' && is_digit(peek_next())) {
    // Consume the "."
    next();

    while (is_digit(peek()))
      next();
  }

  double num = std::stod(std::string(source_.substr(start_, size_())));
  add_token(token_type::NUMBER, num);
}

void scanner::identifier() {
  while (is_alphanumeric(peek()))
    next();

  token_type type = keywords.at(source_.substr(start_, size_()));
  add_token(type);
}

} // namespace lox
