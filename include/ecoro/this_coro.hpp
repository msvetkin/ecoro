// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_THIS_CORO_HPP
#define ECORO_THIS_CORO_HPP

#include "ecoro/task.hpp"
#include "ecoro/executor.hpp"

namespace ecoro::this_coro {

namespace detail {

struct executor_awaiter {
  bool await_ready() const noexcept { return false; }

  template<typename Promise>
  bool await_suspend(std::coroutine_handle<Promise> awaiting_coro) noexcept {
    executor_ = awaiting_coro.promise().executor();
    return false;
  }

  executor *await_resume() const noexcept {
    return executor_;
  }

  executor *executor_{nullptr};
};

} // detail

[[nodiscard]] auto executor() noexcept {
  return detail::executor_awaiter{};
}

template<typename Duration>
[[nodiscard]] task<void> sleep_for(Duration &&duration) {
  auto *e = co_await executor();
  co_await e->sleep_for(std::forward<Duration>(duration));
}

}  // namespace ecoro::this_coro

#endif  // ECORO_THIS_CORO_HPP
