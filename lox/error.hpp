#pragma once

#include <string_view>

namespace lox {

static bool errored;

void error(int line, std::string_view message);

} // namespace lox
