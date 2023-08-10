#include <lox/ast/ast.hpp>
#include <lox/box.hpp>
#include <lox/errors.hpp>
#include <lox/resolver/resolver.hpp>

#include <variant>

namespace lox {

void resolver::resolve(std::vector<stmt> const& stmts) {
  for (stmt const& s : stmts) { std::visit(*this, s); }
}

void resolver::resolve_local(expr e, token name) {
  for (auto it = scopes.crbegin(); it != scopes.crend(); ++it) {
    if (it->contains(name.lexeme)) {
      interpreter_.resolve(e, std::distance(it, scopes.crbegin()));
      return;
    }
  }
}

void resolver::begin_scope() { scopes.emplace_back(); }

void resolver::end_scope() { scopes.pop_back(); }

void resolver::declare(token name) {
  if (scopes.empty()) return;

  scopes.back()[name.lexeme] = false;
}

void resolver::define(token name) {
  if (scopes.empty()) return;

  scopes.back()[name.lexeme] = true;
}

void resolver::operator()(literal_expr const&) {}

void resolver::operator()(variable_expr const& e) {
  if (not scopes.empty() and scopes.back().contains(e.name.lexeme) and
      not scopes.back()[e.name.lexeme]) {
    errors::report(e.name.line,
                   "can't read local variable in its own initialiser");
  }

  resolve_local(e, e.name);
}

void resolver::operator()(box<group_expr> const& e) {
  std::visit(*this, e->ex);
}

void resolver::operator()(box<assign_expr> const& e) {
  std::visit(*this, e->value);
  resolve_local(e, e->name);
}

void resolver::operator()(box<unary_expr> const& e) {
  std::visit(*this, e->right);
}

void resolver::operator()(box<logical_expr> const& e) {
  std::visit(*this, e->left);
  std::visit(*this, e->right);
}

void resolver::operator()(box<binary_expr> const& e) {
  std::visit(*this, e->left);
  std::visit(*this, e->right);
}

void resolver::operator()(box<call_expr> const& e) {
  std::visit(*this, e->callee);

  for (expr arg : e->args) { std::visit(*this, arg); }
}

void resolver::operator()(box<conditional_expr> const& e) {
  std::visit(*this, e->cond);
  std::visit(*this, e->then);
  std::visit(*this, e->alt);
}

void resolver::operator()(expression_stmt const& s) { std::visit(*this, s.ex); }

void resolver::operator()(print_stmt const& s) { std::visit(*this, s.ex); }

void resolver::operator()(variable_stmt const& s) {
  declare(s.name);
  if (s.init) std::visit(*this, *s.init);
  define(s.name);
}

void resolver::operator()(return_stmt const& s) {
  if (s.value) std::visit(*this, *s.value);
}

void resolver::operator()(break_stmt const&) {}

void resolver::operator()(box<block_stmt> const& s) {
  begin_scope();
  resolve(s->stmts);
  end_scope();
}

void resolver::resolve_function(box<function_stmt> const& s) {
  begin_scope();

  for (token param : s->params) {
    declare(param);
    define(param);
  }

  resolve(s->body);

  end_scope();
}

void resolver::operator()(box<function_stmt> const& s) {
  declare(s->name);
  define(s->name);

  resolve_function(s);
}

void resolver::operator()(box<if_stmt> const& s) {
  std::visit(*this, s->cond);
  std::visit(*this, s->then);
  if (s->alt) std::visit(*this, *s->alt);
}

void resolver::operator()(box<while_stmt> const& s) {
  std::visit(*this, s->cond);
  std::visit(*this, s->body);
}

} // namespace lox
