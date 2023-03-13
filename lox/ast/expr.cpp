#include <lox/ast/expr.hpp>

namespace lox {

// clang-format off
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; }; 
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
// clang-format on

auto to_string(literal lit) -> std::string {
  using namespace std::string_literals;
  return std::visit(
      overloaded{[](std::monostate) { return "nil"s; },
                 [](double arg) { return fmt::format("{}", arg); },
                 [](bool arg) { return arg ? "true"s : "false"s; },
                 [](std::string arg) { return fmt::format("\"{}\"", arg); }},
      lit);
}

} // namespace lox
