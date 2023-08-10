#include <lox/errors.hpp>
#include <lox/token/token.hpp>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <iostream>

namespace lox {

parser_error::parser_error(token token, std::string message)
    : token_(std::move(token)), message_(std::move(message)) {}

[[nodiscard]] auto parser_error::what() const noexcept -> const char* {
  return message_.c_str();
}

runtime_error::runtime_error(token token, const std::string& message)
    : std::runtime_error(message), token_(std::move(token)) {}

bool errors::errored         = false;
bool errors::runtime_errored = false;

std::ostream* errors::output = &std::cout;

void errors::report(int line, std::string_view message) {
  fmt::print(*output, "[line {}] Error: {}\n", line, message);
  errored = true;
}

void errors::report_parser_error(const parser_error& err) {
  if (err.token_.type == token_type::EOF) {
    report(err.token_.line, fmt::format("at end: {}", err.message_));
  } else {
    report(err.token_.line,
           fmt::format("at '{}': {}", err.token_.lexeme, err.message_));
  }
}

void errors::report_runtime_error(const runtime_error& err) {
  fmt::print("[line {}] Error: '{}' {}\n", err.token_.line, err.token_.lexeme,
             err.what());
  runtime_errored = true;
}

} // namespace lox
