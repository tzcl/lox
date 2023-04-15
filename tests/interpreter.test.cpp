#include <lox/interpreter/environment.hpp>
#include <lox/interpreter/interpreter.hpp>
#include <lox/parser/parser.hpp>
#include <lox/scanner/scanner.hpp>
#include <tests/util.hpp>

#include <sstream>

TEST_CASE("for statements") {
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
  lox::parser  parser{scanner.scan()};

  std::ostringstream buffer;

  lox::interpreter interpreter{std::make_shared<lox::environment>(), buffer};
  std::vector<lox::stmt> stmts = parser.parse();
  lox::interpret(interpreter, stmts);

  REQUIRE(not buffer.str().empty());
  std::string got = buffer.str();
  REQUIRE(want == got);
}

// TODO: Native functions

TEST_CASE("functions") {
  std::string file;
  std::string want;

  SUBCASE("sayhi") {
    file = "interpreter/sayhi.lox";
    want = "Hi, Dear Reader!\nnil\n";
  }
  SUBCASE("count") {
    file = "interpreter/count.lox";
    want = "1\n2\n3\nnil\n";
  }
  SUBCASE("fib") {
    file = "interpreter/fib.lox";
    want = read_file("interpreter/fib.out");
  }
  SUBCASE("closure") {
    file = "interpreter/closure.lox";
    want = "1\n1\nnil\n2\n2\nnil\n";
  }

  const auto input = read_file(file);

  lox::scanner scanner{input};
  lox::parser  parser{scanner.scan()};

  std::ostringstream buffer;

  lox::interpreter interpreter{std::make_shared<lox::environment>(), buffer};
  std::vector<lox::stmt> stmts = parser.parse();
  lox::interpret(interpreter, stmts);

  REQUIRE(not buffer.str().empty());
  std::string got = buffer.str();
  REQUIRE(want == got);
}
