#include <lox/ast/ast_printer.hpp>
#include <lox/errors.hpp>
#include <lox/interpreter/environment.hpp>
#include <lox/interpreter/interpreter.hpp>
#include <lox/parser/parser.hpp>
#include <lox/scanner/scanner.hpp>
#include <lox/token/token.hpp>

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

static auto run(lox::interpreter& interpreter, std::string const& source)
    -> int {
  lox::scanner scanner(source);
  const auto   tokens = scanner.scan();
  fmt::print("=== Printing tokens ===\n[{}]\n", fmt::join(tokens, ", "));

  // Stop if there was an error
  if (lox::errors::errored) return EX_DATAERR;
  if (lox::errors::runtime_errored) return EX_SOFTWARE;

  lox::parser parser(tokens);
  const auto  stmts = parser.parse();
  fmt::print("=== Printing AST ===\n{}\n",
             fmt::join(lox::print(lox::ast_printer{}, stmts), "\n"));

  fmt::print("=== Evaluating AST ===\n");
  lox::interpret(interpreter, stmts);

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

  lox::interpreter interpreter{};

  int err = run(interpreter, ss.str());
  if (err > 0) return err;

  if (lox::errors::errored) EX_DATAERR;

  return EX_OK;
}

static void run_prompt() {
  fmt::print("Running prompt\n");

  std::string      line;
  lox::interpreter interpreter{};

  while (true) {
    fmt::print("> ");

    if (std::getline(std::cin, line)) {
      run(interpreter, line);

      lox::errors::errored         = false;
      lox::errors::runtime_errored = false;
    } else {
      fmt::print("\n");
      break;
    }
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
