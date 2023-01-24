#include <lox/ast_printer.hpp>
#include <lox/error.hpp>
#include <lox/expr.hpp>
#include <lox/scanner.hpp>
#include <lox/token.hpp>

#include <fmt/core.h>

#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <sysexits.h>

static void run(std::string const& source) {
  lox::scanner scanner(source);

  for (lox::token const& token : scanner.tokens()) {
    fmt::print("{}\n", token.str());
  }
}

// Pass by const reference because we want a non-owning view
// but need a null-terminated string.
static void run_file(std::string const& path) {
  // Requires a null-terminated string (artifact of underlying C file API)
  std::ifstream file(path);
  if (!file.good()) {
    fmt::print("Error opening file: {}\n", path);
    return;
  }

  const std::ostringstream ss;
  file >> ss.rdbuf();

  run(ss.str());

  // TODO: This is not the best, return error codes?
  if (lox::errored) exit(EX_DATAERR);
}

static void run_prompt() {
  fmt::print("Running prompt\n");
  std::string line;
  while (true) {
    fmt::print("> ");
    std::getline(std::cin, line);
    if (line.empty()) break;
    run(line);
    lox::errored = false;
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

  using namespace lox;
  auto ex1 = expr(binary_expr{
      unary_expr{token{token_type::MINUS, "-", 1, {}}, literal_expr{1.}},
      token{token_type::STAR, "*", 1, {}}, grouping_expr{literal_expr{45.67}}});
  auto ex2 = expr(binary_expr{
    binary_expr{literal_expr{1.}, token{token_type::PLUS, "+", 1, {}}, literal_expr{2.}},
    token{token_type::STAR, "*", 1, {}},
    binary_expr{literal_expr{4.}, token{token_type::MINUS, "-", 1, {}}, literal_expr{3.}}
  });

  fmt::print("{}\n", print(sexp_printer{}, ex1));
  fmt::print("{}\n", print(rpn_printer{}, ex2));

  return EX_OK;
}
