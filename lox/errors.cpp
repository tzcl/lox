#include <lox/errors.hpp>

#include <fmt/core.h>

namespace lox::errors {

static bool has_error_;
static bool has_runtime_error_;

void parser_error::anchor() {}
void runtime_error::anchor() {}

auto has_error() -> bool { return has_error_; }

auto has_runtime_error() -> bool { return has_runtime_error_; }

void reset() {
  has_error_         = false;
  has_runtime_error_ = false;
}

void report(int line, std::string_view message) {
  fmt::print("[line {}] Error: {}\n", line, message);
  has_error_ = true;
}

void report_parser_error(parser_error const& err) {
  if (err.token.type == token_type::EOF) {
    report(err.token.line, fmt::format("at end: {}", err.message));
  } else {
    report(err.token.line,
           fmt::format("at '{}': {}", err.token.lexeme, err.message));
  }
}

void report_runtime_error(runtime_error const& err) {
  fmt::print("[line {}] Error: '{}' {}\n", err.token.line, err.token.lexeme,
             err.what());
  has_runtime_error_ = true;
}

} // namespace lox::errors
