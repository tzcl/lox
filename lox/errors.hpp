#pragma once

#include <lox/token.hpp>

#include <stdexcept>
#include <string_view>
#include <utility>

namespace lox::errors {

struct parser_error final : public std::exception {
  token       token;
  std::string message;

  parser_error(lox::token token_, std::string message_)
      : token(std::move(token_)), message(std::move(message_)) {}

  [[nodiscard]] auto what() const noexcept -> const char* override {
    return message.c_str();
  }

private:
  virtual void anchor();
};

struct runtime_error final : public std::runtime_error {
  token token;

  runtime_error(lox::token token_, std::string const& message)
      : std::runtime_error(message), token(std::move(token_)) {}

private:
  virtual void anchor();
};

auto has_error() -> bool;
auto has_runtime_error() -> bool;
void reset();

void report(int line, std::string_view message);
void report_parser_error(parser_error const& err);
void report_runtime_error(runtime_error const& err);

} // namespace lox::errors
