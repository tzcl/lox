#pragma once

#include <lox/ast/ast.hpp>
#include <lox/interpreter/environment.hpp>
#include <lox/interpreter/value.hpp>
#include <lox/token/token.hpp>

#include <iostream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace lox {

class interpreter {
public:
  explicit interpreter(std::ostream& output = std::cout);

  void interpret(std::vector<stmt> const& stmts);

private:
  environment   globals_;
  env_ptr       env;
  std::ostream& output_;
};

} // namespace lox
