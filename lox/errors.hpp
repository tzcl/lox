#pragma once

#include <lox/token/token.hpp>

#include <ostream>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace lox {

struct parser_error final : public std::exception {
  parser_error(token token, std::string message);

  [[nodiscard]] auto what() const noexcept -> const char* override;

  token       token_;
  std::string message_;
};

struct runtime_error final : public std::runtime_error {
  runtime_error(token token, std::string const& message);

  token token_;
};

// Really, this acts like a namespace.
struct errors {
  static void report(int line, std::string_view message);
  static void report_parser_error(const parser_error& err);
  static void report_runtime_error(const runtime_error& err);

  static bool errored;
  static bool runtime_errored;

  static std::ostream* output;
};

} // namespace lox
