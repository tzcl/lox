#pragma once

#include <array>
#include <string>
#include <utility>
#include <variant>

#include <fmt/format.h>

#undef EOF

namespace lox {

// clang-format off
enum class token_type {
  // Single-character tokens
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
  QUESTION, COLON,
  
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
using value = std::variant<std::monostate, bool, double, std::string>;

auto print_value(value literal) -> std::string;
auto to_string(value literal) -> std::string;

struct token {
  token_type type;
  std::string lexeme;
  int line;
  value literal;
  
  [[nodiscard]] auto str() const -> std::string;
};

} // namespace lox

template <> struct fmt::formatter<lox::value> : formatter<std::string> {
  template <typename FormatContext>
  auto format(lox::value const& literal, FormatContext& ctx) const {
    return formatter<std::string>::format(lox::print_value(literal), ctx);
  }
};
  
template <> struct fmt::formatter<lox::token> : formatter<std::string> {
  template <typename FormatContext>
  auto format(lox::token const& token, FormatContext& ctx) const {
    return formatter<std::string>::format(token.str(), ctx);
  }
};
