#pragma once

#include <lox/parser/token.hpp>

#include <string>
#include <unordered_map>
#include <utility>

namespace lox {

struct environment {
  explicit environment(environment* parent) : parent_(parent) {}

  // Use a raw pointer because we want a non-owning pointer that may be null.
  environment* parent_;

  std::unordered_map<std::string, literal> values_;

  void set(std::string name, literal value) {
    values_[name] = std::move(value);
  }
  void assign(token name, literal value);
  auto get(token name) -> literal;
};

} // namespace lox
