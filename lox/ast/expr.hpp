#pragma once

#include <lox/parser/token.hpp>

#include <fmt/format.h>

#include <memory>
#include <string>
#include <variant>

namespace lox {

// TODO: This should be its own layer
using literal = std::variant<std::monostate, bool, double, std::string>;

auto to_string(literal lit) -> std::string;

} // namespace lox

template <>
struct fmt::formatter<lox::literal> : formatter<std::string> {
  template <typename FormatContext>
  auto format(lox::literal const& lit, FormatContext& ctx) const {
    return formatter<std::string>::format(lox::to_string(lit), ctx);
  }
};

namespace lox {

// TODO: Change to use CRTP? (We are not a library)
class expr {
  class base {
  public:
    virtual ~base() = default;

    // clone is like a virtual copy ctor
    [[nodiscard]] virtual auto clone() const -> std::unique_ptr<base> = 0;

    [[nodiscard]] virtual auto eval() const -> literal      = 0;
    [[nodiscard]] virtual auto print() const -> std::string = 0;
  };

  template <typename T>
  class wrapper final : public base {
  public:
    wrapper(T obj) : obj_(std::move(obj)) {}

    [[nodiscard]] auto clone() const -> std::unique_ptr<base> override {
      return std::make_unique<wrapper<T>>(obj_);
    }

    [[nodiscard]] auto eval() const -> literal override { return obj_.eval(); }
    [[nodiscard]] auto print() const -> std::string override {
      return obj_.print();
    }

  private:
    T obj_;
  };

public:
  template <typename T>
  expr(T obj) : ptr_(std::make_unique<wrapper<T>>(std::move(obj))) {}

  expr(const expr& other) : ptr_(other.ptr_->clone()) {}
  auto operator=(const expr& other) -> expr& {
    ptr_ = other.ptr_->clone();
    return *this;
  }

  [[nodiscard]] auto eval() const -> literal { return ptr_->eval(); }
  [[nodiscard]] auto print() const -> std::string { return ptr_->print(); }

private:
  std::unique_ptr<base> ptr_;
};

struct literal_expr {
  literal lit;

  [[nodiscard]] auto eval() const -> literal { return lit; }
  [[nodiscard]] auto print() const -> std::string { return to_string(lit); }
};

struct group_expr {
  expr ex;

  [[nodiscard]] auto eval() const -> literal { return ex.eval(); }
  [[nodiscard]] auto print() const -> std::string {
    return fmt::format("({})", ex.print());
  }
};

struct unary_expr {
  token op;
  expr  right;

  [[nodiscard]] auto eval() const -> literal {
    literal val = right.eval();
    // TODO: implement properly
    return val;
  }

  [[nodiscard]] auto print() const -> std::string {
    return fmt::format("{}{}", op.lexeme, right.print());
  }
};

struct binary_expr {
  expr  left;
  token op;
  expr  right;

  [[nodiscard]] auto eval() const -> literal { return left.eval(); }
  [[nodiscard]] auto print() const -> std::string {
    return fmt::format("{} {} {}", left.print(), op.lexeme, right.print());
  }
};

} // namespace lox
