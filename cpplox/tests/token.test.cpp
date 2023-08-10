#include <lox/token/token.hpp>

#include <doctest/doctest.h>

#include <string>

TEST_CASE("literals") {
  lox::literal literal;
  std::string  want;

  SUBCASE("monostate") {
    literal = std::monostate{};
    want    = "nil";
  }
  SUBCASE("double") {
    literal = 3.1415;
    want    = "3.1415";
  }
  SUBCASE("string") {
    literal = "phenomenal";
    want    = "\"phenomenal\"";
  }

  REQUIRE(want == lox::to_string(literal));
}

TEST_CASE("tokens") {
  lox::token  token;
  std::string want;

  SUBCASE("identifier") {
    token = {lox::token_type::IDENTIFIER, "x", 1};
    want  = "IDENTIFIER";
  }
  SUBCASE("keyword") {
    token = {lox::token_type::PRINT, "print", 1};
    want  = "PRINT";
  }
  SUBCASE("double") {
    token = {lox::token_type::NUMBER, "3.1415", 1, 3.1415};
    want  = "NUMBER \"3.1415\" 3.1415";
  }
  SUBCASE("string") {
    token = {lox::token_type::STRING, "asdf", 1, "asdf"};
    want  = "STRING \"asdf\" \"asdf\"";
  }

  REQUIRE(want == lox::to_string(token));
}