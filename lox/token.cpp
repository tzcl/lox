#include <fmt/std.h>
#include <lox/token.hpp>
#include <utility>

namespace lox {

constexpr std::array token_type_names{
    "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE", "COMMA", "DOT",
    "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR",

    // One or two character tokens
    "BANG", "BANG_EQUAL", "EQUAL", "EQUAL_EQUAL", "GREATER", "GREATER_EQUAL",
    "LESS", "LESS_EQUAL",

    // Literals
    "IDENTIFIER", "STRING", "NUMBER",

    // Keywords
    "AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR", "PRINT",
    "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",

    "EOF"};

// Ensure that all token types can be looked up
static_assert(size(token_type_names) ==
              static_cast<std::size_t>(token_type::NUM_TYPES));

constexpr auto to_string(token_type type) -> const char* {
  return token_type_names.at(static_cast<std::size_t>(type));
}

auto to_string(token_literal literal) -> std::string {
  using namespace std::string_literals;
  return std::visit(
      [](auto const& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
          return "null"s;
        } else if constexpr (std::is_same_v<T, std::string>) {
          return arg;
        } else if constexpr (std::is_same_v<T, double>) {
          return fmt::format("{}", arg);
        }
      },
      literal);
}

token::token(token_type type, std::string lexeme, int line,
             token_literal literal)
    : type(type), lexeme(std::move(lexeme)), line(line),
      literal(std::move(literal)) {}

auto token::str() const -> std::string {
  return fmt::format("{} {} {}", to_string(type), lexeme, literal);
}
} // namespace lox
