#pragma once

#include <lox/token/token.hpp>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace lox {

class scanner {
public:
  explicit scanner(std::string source) : source_(std::move(source)) {}

  auto scan() -> std::vector<token>;

private:
  void scan_next();

  void string();
  void number();
  void identifier();
  void block_comment();

  inline auto next() -> char { return source_[curr_++]; }
  inline auto done() -> bool { return curr_ >= ssize(source_); }

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
  inline void add_token(token_type type, const literal& lit = literal{}) {
    // Clang 15 doesn't support parenthesised aggregate initialisation :(
    // tokens_.emplace_back(type, substr(start_, curr_), line_, literal);
    tokens_.push_back(token{type, substr(start_, curr_), line_, lit});
  }
  inline auto substr(int start, int end) -> std::string {
    return source_.substr(start, end - start);
  }

  const std::string  source_;
  std::vector<token> tokens_;

  int start_ = 0;
  int curr_  = 0;
  int line_  = 1;
};

} // namespace lox
