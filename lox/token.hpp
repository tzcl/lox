#pragma once

#include <string>
#include <variant>

namespace lox {

// clang-format off
enum class token_type {
  // Single-character tokens
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
  
  // One or two character tokens
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,
  
  // Literals
  IDENTIFIER, STRING, NUMBER,
  
  // Keywords
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
  
  EOF
};
// clang-format on

using token_literal = std::variant<std::string, int>;

struct token {
  token_type const type_;
  std::string const lexeme_;
  token_literal const literal_;
  int const line_;

  token(token_type type, std::string lexeme, token_literal literal, int line);

  [[nodiscard]] auto str() const -> std::string;
};

} // namespace lox