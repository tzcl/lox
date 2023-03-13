#include <lox/errors.hpp>
#include <lox/interpreter/value.hpp>

namespace lox {

// clang-format off
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; }; 
// Still need deduction guide with Clang 15
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
// clang-format on

auto to_string(value value) -> std::string {
  using namespace std::string_literals;
  return std::visit(
      overloaded{[](literal& literal) { return to_string(literal); },
                 [](callable& callable) {
                   return fmt::format("<fn {}>", callable.decl.name.lexeme);
                 }},
      value);
}

auto is_truthy(value value) -> bool {
  return std::visit(overloaded{
                        [](literal& literal) { return is_truthy(literal); },
                        [](callable&) { return true; },
                    },
                    value);
}

auto get_literal(token token, value value) -> literal {
  try {
    return std::get<literal>(value);
  } catch (std::bad_variant_access&) {
    throw errors::runtime_error(std::move(token), "expected literal");
  }
}

} // namespace lox
