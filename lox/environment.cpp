#include <lox/environment.hpp>

#include <lox/errors.hpp>

#include <fmt/ranges.h>
#include <utility>

namespace lox {

void environment::assign(token name, value value) {
  if (values_.contains(name.lexeme)) {
    values_[name.lexeme] = std::move(value);
    return;
  }

  if (parent_ != nullptr) {
    parent_->assign(name, value);
    return;
  }

  throw errors::runtime_error(
      name, fmt::format("Undefined variable '{}'", name.lexeme));
}

auto environment::get(token name) -> value {
  if (values_.contains(name.lexeme)) return values_[name.lexeme];

  if (parent_ != nullptr) return parent_->get(name);

  throw errors::runtime_error(
      name, fmt::format("Undefined variable '{}'", name.lexeme));
}

} // namespace lox
