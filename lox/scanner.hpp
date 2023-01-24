#pragma once

#include <lox/token.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace lox {

class scanner {
public:
  explicit scanner(std::string source) : source_(std::move(source)) {}

  auto tokens() -> std::vector<token>;

private:
  void scan();

  void string();
  void number();
  void identifier();

  inline auto next() -> char { return source_[curr_++]; }
  inline auto peek() -> char {
    if (done()) return '\0';
    return source_[curr_];
  }
  inline auto peek_next() -> char {
    if (curr_ + 1 >= ssize(source_)) return '\0';
    return source_[curr_ + 1];
  }
  inline auto match(char c) -> bool {
    if (done() || source_[curr_] != c) return false;
    ++curr_;
    return true;
  }
  inline auto done() -> bool { return curr_ >= ssize(source_); }

  inline void add_token(token_type type,
                        token_literal const& literal = token_literal()) {
    // Clang 15 doesn't support parenthesised aggregate initialisation :(
    // tokens_.emplace_back(type, substr(start_, curr_), line_, literal);
    tokens_.push_back(token{type, substr(start_, curr_), line_, literal});
  }

  inline auto substr(int start, int end) -> std::string {
    return source_.substr(start, end - start);
  }

  static inline auto is_digit(char c) -> bool { return c >= '0' && c <= '9'; }
  static inline auto is_alpha(char c) -> bool {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }
  static inline auto is_alphanumeric(char c) -> bool {
    return is_alpha(c) || is_digit(c);
  }

  void skip_block_comment();

  std::string const source_;
  std::vector<token> tokens_;

  int start_ = 0;
  int curr_ = 0;
  int line_ = 1;

  const std::unordered_map<std::string_view, token_type> keywords = {{
      {"and", token_type::AND},
      {"class", token_type::CLASS},
      {"else", token_type::ELSE},
      {"false", token_type::FALSE},
      {"for", token_type::FOR},
      {"fun", token_type::FUN},
      {"if", token_type::IF},
      {"nil", token_type::NIL},
      {"or", token_type::OR},
      {"print", token_type::PRINT},
      {"return", token_type::RETURN},
      {"super", token_type::SUPER},
      {"this", token_type::THIS},
      {"true", token_type::TRUE},
      {"var", token_type::VAR},
      {"while", token_type::WHILE},
  }};
};

} // namespace lox
