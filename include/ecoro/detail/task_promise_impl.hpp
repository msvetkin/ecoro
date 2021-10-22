// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_DETAIL_TASK_PROMISE_IMPL_HPP
#define ECORO_DETAIL_TASK_PROMISE_IMPL_HPP

#include "ecoro/coroutine.hpp"

#include <exception>
#include <variant>

namespace ecoro::detail {

template<typename T>
class task_promise_impl {
  static constexpr bool is_ref = std::is_reference_v<T>;
  using result_variant_value_type =
      std::conditional_t<is_ref, std::add_pointer_t<T>, T>;

 public:
  using value_type = T;

  void unhandled_exception() noexcept {
    result_variant_ = std::current_exception();
  }

  template<typename U>
  void return_value(U &&value) noexcept {
    if constexpr (is_ref) {
      result_variant_ = &value;
    } else {
      result_variant_ = std::forward<U>(value);
    }
  }

  value_type result() {
    if (auto exception = std::get_if<std::exception_ptr>(&result_variant_)) {
      std::rethrow_exception(*exception);
    }

    if constexpr (is_ref) {
      return *std::get<result_variant_value_type>(result_variant_);
    } else {
      return std::get<result_variant_value_type>(std::move(result_variant_));
    }
  }

 private:
  using variant = std::variant<std::monostate, result_variant_value_type,
                               std::exception_ptr>;
  variant result_variant_;
};

template<>
class task_promise_impl<void> {
 public:
  using value_type = void;

  void return_void() const noexcept {}

  void result() {
    if (exception_) {
      std::rethrow_exception(exception_);
    }
  }

  void unhandled_exception() noexcept {
    exception_ = std::current_exception();
  }

 private:
  std::exception_ptr exception_;
};

}  // namespace ecoro::detail

#endif  // ECORO_DETAIL_TASK_PROMISE_IMPL_HPP
