#include <lox/errors.hpp>
#include <lox/parser/scanner.hpp>
#include <lox/parser/token.hpp>

#include <fmt/ranges.h>

#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <sysexits.h>
#include <utility>
#include <vector>

static auto run(std::string const& source) -> int {
  lox::scanner scanner(source);

  // lox::parser parser(scanner.tokens());
  // std::vector<lox::stmt> stmts = parser.parse();

  // Stop if there was an error
  if (lox::errors::has_error()) return EX_DATAERR;
  if (lox::errors::has_runtime_error()) return EX_SOFTWARE;

  // lox::interpret(interpreter, stmts);

  fmt::print("{}\n", scanner.tokens());

  return EX_OK;
}

// Pass by const reference because we want a non-owning view
// but need a null-terminated string.
static auto run_file(std::string const& path) -> int {
  // Requires a null-terminated string (artifact of underlying C file API)
  std::ifstream file(path);
  if (!file.good()) {
    fmt::print("Error opening file: {}\n", path);
    return EX_NOINPUT;
  }

  const std::ostringstream ss;
  file >> ss.rdbuf();

  // lox::interpreter interpreter{lox::environment(nullptr)};

  int err = run(ss.str());
  if (err > 0) return err;

  if (lox::errors::has_error()) EX_DATAERR;

  return EX_OK;
}

static void run_prompt() {
  fmt::print("Running prompt\n");
  std::string line;
  // lox::interpreter interpreter{lox::environment(nullptr)};
  while (true) {
    fmt::print("> ");
    std::getline(std::cin, line);
    if (!std::cin) break;

    run(line);

    lox::errors::reset();
  }
}

auto main(int argc, char* argv[]) -> int {
  if (argc > 2) {
    fmt::print("Usage: lox [script]\n");
    return EX_USAGE;
  } else if (argc == 2) {
    int err = run_file(argv[1]); // NOLINT
    if (err > 0) return err;
  } else {
    run_prompt();
  }

  return EX_OK;
}
