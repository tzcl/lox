#pragma once

#include <lox/token.hpp>

#include <string>
#include <unordered_map>
#include <utility>

namespace lox {

struct environment {
  // Use raw string instead of a token because we don't care about the source
  // code.
  std::unordered_map<std::string, value> values;

  void set(std::string name, value value);
  void assign(token name, value value);
  auto get(token name) -> value;
};

} // namespace lox
