#include <lox/ast/expr.hpp>
#include <tests/util.hpp>

#include <fmt/core.h>

TEST_CASE("handrolled") {
  lox::expr expression(lox::binary_expr{
      lox::unary_expr{lox::token{lox::token_type::MINUS, "-", 1},
                      lox::literal_expr{123.}},
      lox::token{lox::token_type::STAR, "*", 1},
      lox::group_expr{lox::literal_expr{45.67}}});

  std::string want = "-123 * (45.67)";
  std::string got  = expression.print();
  CHECK(want == got);
}