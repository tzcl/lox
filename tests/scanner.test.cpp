#include <lox/parser/scanner.hpp>

#include "doctest/doctest.h"
#include <fmt/core.h>

#include <iostream>
#include <string>
#include <vector>

namespace doctest {
template <>
struct StringMaker<lox::literal> {
  static auto convert(const lox::literal& value) -> String {
    return fmt::format("{}", value).c_str();
  }
};
} // namespace doctest

using enum lox::token_type;

using namespace std::string_literals;

void tokens_equal(const std::vector<lox::token>& want,
                  const std::vector<lox::token>& got) {
  REQUIRE(std::size(want) == std::size(got));

  for (int i = 0; i < std::ssize(want); ++i) {
    CHECK(want[i].type == got[i].type);
    CHECK(want[i].lexeme == got[i].lexeme);
    CHECK(want[i].line == got[i].line);
    CHECK(want[i].literal == got[i].literal);
  }
}

TEST_CASE("basic") {
  const auto input = R"(print "Hello, world" 42 1.333 true)"s;

  lox::scanner scanner{input};
  const auto   got = scanner.tokens();

  using enum lox::token_type;
  std::vector<lox::token> want{
      lox::token{PRINT, "print", 1},
      lox::token{STRING, "\"Hello, world\"", 1, "Hello, world"s},
      lox::token{NUMBER, "42", 1, 42.},
      lox::token{NUMBER, "1.333", 1, 1.333},
      lox::token{TRUE, "true", 1},
      lox::token{EOF, "", 1},
  };

  tokens_equal(want, got);
}
