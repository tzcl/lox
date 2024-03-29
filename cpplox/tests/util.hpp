#pragma once

#include <lox/token/token.hpp>

#include <doctest/doctest.h>
#include <fmt/core.h>

#include <fstream>
#include <sstream>
#include <string_view>
#include <vector>

namespace doctest {

template <>
struct StringMaker<lox::literal> {
  static auto convert(const lox::literal& literal) -> String {
    return fmt::format("{}", literal).c_str();
  }
};

template <>
struct StringMaker<lox::token> {
  static auto convert(const lox::token& token) -> String {
    return fmt::format("{}", token).c_str();
  }
};

} // namespace doctest

void tokens_equal(const std::vector<lox::token>& want,
                  const std::vector<lox::token>& got);

auto read_file(std::string_view path) -> std::string;
