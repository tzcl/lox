#include <lox/environment.hpp>

#include <lox/errors.hpp>

#include <fmt/ranges.h>
#include <utility>

namespace lox {

void environment::set(std::string name, value value) {
  values[name] = std::move(value);
}

void environment::assign(token name, value value) {
  if (values.contains(name.lexeme)) {
    values[name.lexeme] = std::move(value);
    return;
  }

  throw errors::runtime_error(
      name, fmt::format("Undefined variable '{}'", name.lexeme));
}

auto environment::get(token name) -> value {
  if (values.contains(name.lexeme)) return values[name.lexeme];

  throw errors::runtime_error(
      name, fmt::format("Undefined variable '{}'", name.lexeme));
}

} // namespace lox
