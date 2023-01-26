#include <lox/token.hpp>

#include <fmt/std.h>

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

// clang-format off
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; }; 
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
// clang-format on

auto to_string(token_literal literal) -> std::string {
  using namespace std::string_literals;
  return std::visit(
      overloaded{[](std::monostate) { return "null"s; },
                 [](bool arg) { return arg ? "true"s : "false"s; },
                 [](double arg) { return fmt::format("{}", arg); },
                 [](std::string arg) { return fmt::format("\"{}\"", arg); }},
      literal);
}

auto token::str() const -> std::string {
  return literal.index() == 0U
             ? fmt::format("{}", to_string(type))
             : fmt::format("{} {} {}", to_string(type), lexeme, literal);
}

} // namespace lox
