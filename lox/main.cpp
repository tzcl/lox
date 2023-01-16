#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <sysexits.h>

#include <lox/token.hpp>

void run(std::string_view source) {
  // lox::scanner scanner(source);

  // for (lox::token const& token : scanner.tokens()) {
  //   fmt::print("{}\n", token);
  // }
}

// Pass by const reference because we want a non-owning view
// but need a null-terminated string.
void run_file(std::string const& path) {
  // Requires a null-terminated string (artifact of underlying C file API)
  std::ifstream file(path);
  if (!file.good()) {
    fmt::print("Error opening file: {}\n", path);
    return;
  }

  const std::ostringstream ss;
  file >> ss.rdbuf();

  run(ss.str());
}

void run_prompt() {
  fmt::print("Running prompt\n");
  std::string line;
  while (true) {
    fmt::print("> ");
    std::getline(std::cin, line);
    if (line.empty()) break;
    run(line);
  }
}

auto main(int argc, char* argv[]) -> int {
  fmt::print("hello, world!\n");
  fmt::print("{}, {}\n", argc, argc > 0 ? argv[0] : "");

  if (argc > 2) {
    fmt::print("Usage: lox [script]\n");
    return EX_USAGE;
  } else if (argc == 2) {
    run_file(argv[0]);
  } else {
    run_prompt();
  }

  return EX_OK;
}