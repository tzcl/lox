#pragma once

#include <memory>

namespace lox {

// box is a wrapper for unique_ptr that gives it value semantics.
// Inspired by https://www.foonathan.net/2022/05/recursive-variant-box/
template <typename T> class box {
  std::unique_ptr<T> impl_;

public:
  // Automatic construction from a `T`, not a `T*`
  box(T&& obj) : impl_(new T(std::move(obj))) {}
  box(const T& obj) : impl_(new T(obj)) {}

  // Copy constructor copies `T`
  box(const box& other) : box(*other.impl_) {}
  auto operator=(const box& other) -> box& {
    *impl_ = *other.impl_;
    return *this;
  }

  // Omit move constructor to avoid exceptions/invalid state

  // unique_ptr deletes `T` for us
  ~box() = default;

  // Access propagates constness
  auto operator*() -> T& { return *impl_; }
  auto operator*() const -> const T& { return *impl_; }

  auto operator->() -> T* { return impl_.get(); }
  auto operator->() const -> const T* { return impl_.get(); }
};

} // namespace lox
