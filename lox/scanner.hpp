#pragma once

#include <lox/token.hpp>
#include <string>
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
  inline auto done() -> bool { return curr_ >= ssize(source_); };

  inline void add_token(token_type type,
                        token_literal const& literal = token_literal()) {
    tokens_.emplace_back(type, substr(start_, curr_), line_,
                         literal);
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

  std::string const source_;
  std::vector<token> tokens_;

  int start_ = 0;
  int curr_ = 0;
  int line_ = 1;
};
} // namespace lox
