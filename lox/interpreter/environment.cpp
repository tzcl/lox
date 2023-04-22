#include <lox/errors.hpp>
#include <lox/interpreter/environment.hpp>
#include <utility>

#include <fmt/format.h>
#include <fmt/ranges.h>

namespace lox {

auto environment::ancestor(int dist) -> environment& {
  environment* env = this;
  for (int i = 0; i < dist; ++i) { env = env->parent_.get(); }

  return *env;
}

void environment::assign(token name, value value) {
  if (values_.contains(name.lexeme)) {
    values_[name.lexeme] = std::move(value);
    return;
  }

  if (parent_ != nullptr) {
    parent_->assign(name, value);
    return;
  }

  throw runtime_error(name,
                      fmt::format("undefined variable '{}'", name.lexeme));
}

void environment::assign(int dist, token name, value value) {
  ancestor(dist).values_[name.lexeme] = std::move(value);
}

auto environment::get(token name) -> value {
  if (values_.contains(name.lexeme)) return values_[name.lexeme];

  if (parent_ != nullptr) return parent_->get(name);

  throw runtime_error(name,
                      fmt::format("undefined variable '{}'", name.lexeme));
}

auto environment::get(int dist, token name) -> value {
  return ancestor(dist).values_[name.lexeme];
}

} // namespace lox
