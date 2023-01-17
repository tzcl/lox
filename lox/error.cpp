#include <fmt/core.h>
#include <lox/error.hpp>

namespace lox {

void error(int line, std::string_view message) {
  fmt::print("[line {}] Error: {}\n", line, message);
  errored = true;
}

} // namespace lox
