#include <lox/error.hpp>

#include <fmt/core.h>

namespace lox::error {

void report(int line, std::string_view message) {
  fmt::print("[line {}] Error {}\n", line, message);
  errored = true;
}

void parser_err(token token, std::string_view message) {
  if (token.type == token_type::EOF) {
    report(token.line, fmt::format("at end: {}", message));
  } else {
    report(token.line, fmt::format("at '{}': {}", token.lexeme, message));
  }
}

} // namespace lox
