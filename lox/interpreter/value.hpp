#pragma once

#include <lox/box.hpp>
#include <lox/grammar.hpp>
#include <lox/parser/token.hpp>

#include <variant>
#include <vector>

namespace lox {

using value = std::variant<literal, struct callable>;

template <typename T>
concept visitor =
    requires(T& t, const std::vector<stmt>& stmts) { interpret(t, stmts); };

// TODO: Type erasure?
struct callable {
  void call(visitor auto visitor, const std::vector<value>& values);
  auto arity() -> int;
};

} // namespace lox
