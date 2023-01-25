#pragma once

#include <lox/expr.hpp>
#include <lox/token.hpp>

#include <utility>
#include <vector>

namespace lox {

class parser {
public:
  explicit parser(std::vector<token> tokens) : tokens_(std::move(tokens)) {}

private:
  auto expression() -> expr;
  auto equality() -> expr;
  auto comparison() -> expr;
  auto term() -> expr;
  auto factor() -> expr;
  auto unary() -> expr;
  auto primary() -> expr;

  auto left_assoc(auto&& rule, std::initializer_list<token_type> types) -> expr;
  auto match(std::initializer_list<token_type> types) -> bool;

  inline auto peek() -> token { return tokens_[curr_]; }
  inline auto prev() -> token { return tokens_[curr_ - 1]; }
  inline auto done() -> bool { return peek().type == token_type::EOF; }
  inline auto next() -> token {
    if (!done()) ++curr_;
    return prev();
  }
  inline auto check(token_type type) -> bool {
    if (done()) return false;
    return peek().type == type;
  }

  std::vector<token> tokens_;
  int curr_ = 0;
};

} // namespace lox
