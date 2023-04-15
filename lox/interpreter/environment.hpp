#pragma once

#include <lox/interpreter/value.hpp>
#include <lox/token/token.hpp>

#include <string>
#include <unordered_map>
#include <utility>

#include <fmt/format.h>

namespace lox {

class environment {
public:
  explicit environment(environment* parent = nullptr) : parent_(parent) {}

  void define(std::string name, value value) {
    values_[name] = std::move(value);
  }
  void assign(token name, value value);
  auto get(token name) -> value;

  // private:
  // Use a raw pointer because we want a non-owning pointer that may be null.
  environment* parent_;

  std::unordered_map<std::string, value> values_;
};

} // namespace lox
