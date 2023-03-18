#pragma once

#include <lox/errors.hpp>
#include <lox/token/token.hpp>

#include <memory>
#include <utility>
#include <variant>

namespace lox {

class value final {
private:
  class base {
  public:
    virtual ~base() = default;

    [[nodiscard]] virtual auto clone() const -> std::unique_ptr<base> = 0;

    [[nodiscard]] virtual auto is_truthy() const -> bool = 0;

    [[nodiscard]] virtual auto get_double(token op) const -> double = 0;

    friend constexpr auto operator==(const base&, const base&)
        -> bool = default;
  };

  template <typename T>
  class model final : public base {
  public:
    model(T obj) : obj_(std::move(obj)) {}

    [[nodiscard]] auto clone() const -> std::unique_ptr<base> override {
      return std::make_unique<model<T>>(obj_);
    }

    [[nodiscard]] auto is_truthy() const -> bool override {
      return obj_.is_truthy();
    }

    [[nodiscard]] auto get_double(token op) const -> double override {
      return obj_.get_double(std::move(op));
    }

  private:
    T obj_;
  };

  std::unique_ptr<base> ptr_;

public:
  template <typename T>
  value(T obj) : ptr_(std::make_unique<model<T>>(std::move(obj))) {}

  value(const value& other) : ptr_(other.ptr_->clone()) {}
  auto operator=(const value& other) -> value& {
    value tmp(other);
    std::swap(ptr_, tmp.ptr_);
    return *this;
  }

  // Leaving the move constructor undefined means that the copy constructor
  // will be used as a fallback. This means that we avoid the moved-from state.
  // value(value&& other);
  auto operator=(value&& other) noexcept -> value& {
    ptr_.swap(other.ptr_);
    return *this;
  }

  [[nodiscard]] auto is_truthy() const -> bool { return ptr_->is_truthy(); }

  [[nodiscard]] auto get_double(token op) const -> double {
    return ptr_->get_double(std::move(op));
  }

  friend auto operator==(const value&, const value&) -> bool  = default;
  friend auto operator<=>(const value&, const value&) -> bool = default;
};

struct primitive {
  literal lit;

  [[nodiscard]] auto is_truthy() const -> bool { return lit.index() != 0; }

  [[nodiscard]] auto get_double(token op) const -> double {
    try {
      return std::get<double>(lit);
    } catch (std::bad_variant_access&) {
      throw runtime_error(std::move(op), "operand must be a number");
    }
  }

  friend constexpr auto operator==(const primitive& a, const primitive& b)
      -> bool {
    return true;
  }
};

} // namespace lox
