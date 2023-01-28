#pragma once

#include <lox/token.hpp>

#include <string_view>

namespace lox::errors {

static bool errored;

void report(int line, std::string_view message);
void parser_err(token token, std::string_view message);

} // namespace lox::error
