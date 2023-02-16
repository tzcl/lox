#pragma once

#include <lox/grammar.hpp>
#include <lox/token.hpp>

#include <string_view>
#include <utility>
#include <vector>

namespace lox {

class parser {
public:
  explicit parser(std::vector<token> tokens) : tokens_(std::move(tokens)) {}

  auto parse() -> std::vector<stmt>;

private:
  auto declaration() -> stmt;
  auto var_declaration() -> stmt;
  auto statement() -> stmt;
  auto print_statement() -> stmt;
  auto expression_statement() -> stmt;
  auto block_statement() -> std::vector<stmt>;
  auto expression() -> expr;
  auto assignment() -> expr;
  auto comma() -> expr;
  auto conditional() -> expr;
  auto equality() -> expr;
  auto comparison() -> expr;
  auto term() -> expr;
  auto factor() -> expr;
  auto unary() -> expr;
  auto primary() -> expr;

  void missing_binary_op();

  template <typename R>
  auto left_assoc(R rule, std::initializer_list<token_type> types) -> expr;

  auto match(std::initializer_list<token_type> types) -> bool;
  auto consume(token_type type, std::string_view message) -> token;
  void synchronise();

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

  const std::vector<token> tokens_;

  int curr_ = 0;
};

} // namespace lox
