#include <lox/interpreter/environment.hpp>
#include <lox/interpreter/interpreter.hpp>
#include <lox/parser/parser.hpp>
#include <lox/scanner/scanner.hpp>
#include <tests/util.hpp>

#include <sstream>

TEST_CASE("interpreter") {
  std::string input;
  std::string want;

  SUBCASE("loop 0 to 9") {
    input = read_file("interpreter/for.lox");
    want  = read_file("interpreter/for.out");
  }
  SUBCASE("loop break") {
    input = read_file("interpreter/break.lox");
    want  = read_file("interpreter/break.out");
  }
  SUBCASE("sayhi") {
    input = read_file("interpreter/sayhi.lox");
    want  = "Hi, Dear Reader!\nnil\n";
  }
  SUBCASE("count") {
    input = read_file("interpreter/count.lox");
    want  = "1\n2\n3\nnil\n";
  }
  SUBCASE("fib") {
    input = read_file("interpreter/fib.lox");
    want  = read_file("interpreter/fib.out");
  }
  SUBCASE("closure") {
    input = read_file("interpreter/closure.lox");
    want  = "1\n1\nnil\n2\n2\nnil\n";
  }
  SUBCASE("native fn") {
    input = R"(min("a", "b");)";
    want  = "a\n";
  }

  lox::scanner scanner{input};
  lox::parser  parser{scanner.scan()};

  std::ostringstream buffer;

  lox::interpreter       interpreter{buffer};
  std::vector<lox::stmt> stmts = parser.parse();
  interpreter.interpret(stmts);

  REQUIRE(not buffer.str().empty());
  std::string got = buffer.str();
  REQUIRE(want == got);
}
