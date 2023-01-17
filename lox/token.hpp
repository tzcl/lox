#pragma once

#include <array>
#include <string>
#include <variant>

#undef EOF

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
  
  EOF,
  
  NUM_TYPES
};

// std::monostate here is a well-behaved empty alternative. Putting it first
// allows for token_literal to be default-constructed.
using token_literal = std::variant<std::monostate, std::string, double>;

struct token {
  token_type const type_;
  std::string const lexeme_;
  int const line_;
  token_literal const literal_;

  token(token_type type, std::string lexeme, int line,
        token_literal literal = token_literal());

  [[nodiscard]] auto str() const -> std::string;
};

} // namespace lox