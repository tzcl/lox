#include <lox/parser/token.hpp>

#include <fmt/std.h>

#include <utility>

namespace lox {

constexpr std::array token_type_names{
    "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE", "COMMA", "DOT",
    "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR", "QUESTION", "COLON",

    // One or two character tokens
    "BANG", "BANG_EQUAL", "EQUAL", "EQUAL_EQUAL", "GREATER", "GREATER_EQUAL",
    "LESS", "LESS_EQUAL",

    // Literals
    "IDENTIFIER", "STRING", "NUMBER",

    // Keywords
    "AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR", "PRINT",
    "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE", "BREAK",

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

auto print_value(token_value value) -> std::string {
  using namespace std::string_literals;
  return std::visit(
      overloaded{[](std::monostate) { return "nil"s; },
                 [](double arg) { return fmt::format("{}", arg); },
                 [](std::string arg) { return fmt::format("\"{}\"", arg); }},
      value);
}

auto to_string(token_value value) -> std::string {
  using namespace std::string_literals;
  return std::visit(
      overloaded{[](std::monostate) { return ""s; },
                 [](double arg) { return fmt::format("{}", arg); },
                 [](std::string arg) { return fmt::format("\"{}\"", arg); }},
      value);
}

auto token::str() const -> std::string {
  return value.index() == 0U
           ? fmt::format("{}", to_string(type))
           : fmt::format("{} {} {}", to_string(type), lexeme, value);
}

} // namespace lox
