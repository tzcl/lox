#include <fmt/std.h>
#include <lox/token.hpp>
#include <utility>

namespace lox {
token::token(token_type type, std::string lexeme, int line,
             token_literal literal)
    : type_(type), lexeme_(std::move(lexeme)), line_(line),
      literal_(std::move(literal)) {}

auto token::str() const -> std::string {
  return fmt::format("{} {} {}", static_cast<int>(type_), lexeme_, literal_);
}
} // namespace lox
