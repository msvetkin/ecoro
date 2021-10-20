// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_SCOPE_GUARD_HPP_
#define ECORO_SCOPE_GUARD_HPP_

#include <functional>
#include <memory>
#include <utility>

namespace ecoro {

class scope_guard {
 public:
  template <typename Callable>
  explicit scope_guard(Callable &&callback) noexcept
      : callback_(std::forward<Callable>(callback)) {}

  template <typename Context, typename Callable>
  explicit scope_guard(Context *context, Callable &&callback) noexcept
      : callback_(std::bind(callback, context)) {}

  scope_guard(scope_guard &&other) noexcept
      : callback_(std::exchange(other.callback_, nullptr)) {}

  scope_guard &operator=(scope_guard &&other) noexcept {
    if (std::addressof(other) != this) {
      callback_ = std::exchange(other.callback_, {});
    }

    return *this;
  }

  ~scope_guard() {
    if (callback_) {
      callback_();
    }
  }

  scope_guard(const scope_guard &) = delete;
  scope_guard operator=(const scope_guard &) = delete;

 private:
  std::function<void()> callback_;
};

}  // namespace ecoro

#endif  // ECORO_SCOPE_GUARD_HPP_
