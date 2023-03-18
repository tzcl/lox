#pragma once

#include <lox/ast/box.hpp>
#include <lox/token/token.hpp>

#include <optional>
#include <variant>
#include <vector>

namespace lox {

// C++ is value-oriented whereas Java/Python are reference-oriented.
// > You cannot overwrite a const object and refer to it by the same name
// > afterwards.
// https://stackoverflow.com/questions/58414966/move-construction-and-assignment-of-class-with-constant-member

struct literal_expr {
  literal value;
};

struct variable_expr {
  token name;
};

using expr = std::variant<literal_expr, variable_expr, box<struct group_expr>,
                          box<struct assign_expr>, box<struct unary_expr>,
                          box<struct logical_expr>, box<struct binary_expr>,
                          box<struct conditional_expr>>;

struct group_expr {
  expr ex;
};

struct assign_expr {
  token name;
  expr  value;
};

struct unary_expr {
  token op;
  expr  right;
};

struct logical_expr {
  expr  left;
  token op;
  expr  right;
};

struct binary_expr {
  expr  left;
  token op;
  expr  right;
};

struct conditional_expr {
  expr cond;
  expr then;
  expr alt;
};

struct expression_stmt {
  expr ex;
};

struct print_stmt {
  expr ex;
};

struct variable_stmt {
  token               name;
  std::optional<expr> init;
};

struct break_stmt {
  int loop_depth;
};

using stmt = std::variant<expression_stmt, print_stmt, variable_stmt,
                          break_stmt, struct block_stmt, box<struct if_stmt>,
                          box<struct while_stmt>>;

struct block_stmt {
  std::vector<stmt> stmts;
};

struct function_stmt {
  token              name;
  std::vector<token> params;
  std::vector<stmt>  body;
};

struct if_stmt {
  expr                cond;
  stmt                then;
  std::optional<stmt> alt;
};

struct while_stmt {
  expr cond;
  stmt body;
};

} // namespace lox
