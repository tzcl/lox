#include <tests/util.hpp>

void tokens_equal(const std::vector<lox::token>& want,
                  const std::vector<lox::token>& got) {
  REQUIRE(std::size(want) == std::size(got));

  for (int i = 0; i < std::ssize(want); ++i) {
    CAPTURE(i);
    CAPTURE(want[i]);
    CAPTURE(got[i]);
    CHECK(want[i].type == got[i].type);
    CHECK(want[i].lexeme == got[i].lexeme);
    CHECK(want[i].line == got[i].line);
    CHECK(want[i].literal == got[i].literal);
  }
}

auto read_file(std::string_view path) -> std::string {
  std::ifstream file(fmt::format("testdata/{}", path));
  REQUIRE(file.good());

  std::ostringstream ss;
  file >> ss.rdbuf();

  return ss.str();
}
