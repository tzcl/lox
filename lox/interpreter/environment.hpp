#pragma once

#include <lox/interpreter/value.hpp>
#include <lox/token/token.hpp>

#include <fmt/format.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace lox {

class environment {
public:
  explicit environment(std::shared_ptr<environment> parent = nullptr)
      : parent_(std::move(parent)) {}

  void define(std::string name, value value) {
    values_[name] = std::move(value);
  }
  void assign(token name, value value);
  void assign(int dist, token name, value value);
  auto get(token name) -> value;
  auto get(int dist, token name) -> value;

  // private:
  std::shared_ptr<environment> parent_;

  std::unordered_map<std::string, value> values_;

  auto ancestor(int dist) -> environment&;
};

} // namespace lox
