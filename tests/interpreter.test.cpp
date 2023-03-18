#include <lox/interpreter/environment.hpp>
#include <lox/interpreter/interpreter.hpp>
#include <lox/parser/parser.hpp>
#include <lox/scanner/scanner.hpp>
#include <tests/util.hpp>

#include <sstream>

TEST_CASE("for statement") {
  std::string file;
  std::string expected;

  SUBCASE("0 to 9") {
    file     = "interpreter/for.lox";
    expected = "interpreter/for.out";
  }
  SUBCASE("break") {
    file     = "interpreter/break.lox";
    expected = "interpreter/break.out";
  }

  const auto input = read_file(file);
  const auto want  = read_file(expected);

  lox::scanner scanner{input};
  lox::parser  parser{scanner.tokens()};

  std::ostringstream buffer;

  lox::interpreter       interpreter{lox::environment(nullptr), buffer};
  std::vector<lox::stmt> stmts = parser.parse();
  lox::interpret(interpreter, stmts);

  REQUIRE(not buffer.str().empty());
  std::string got = buffer.str();
  REQUIRE(want == got);
}