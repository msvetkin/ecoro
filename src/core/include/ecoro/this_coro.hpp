// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_THIS_CORO_HPP
#define ECORO_THIS_CORO_HPP

#include "ecoro/scheduler.hpp"
#include "ecoro/task.hpp"

namespace ecoro::this_coro {

namespace detail {

struct scheduler_awaiter {
  bool await_ready() const noexcept {
    return false;
  }

  template<typename Promise>
  bool await_suspend(std::coroutine_handle<Promise> awaiting_coro) noexcept {
    scheduler_ = awaiting_coro.promise().scheduler();
    return false;
  }

  scheduler *await_resume() const noexcept {
    return scheduler_;
  }

  scheduler *scheduler_{nullptr};
};

}  // namespace detail

[[nodiscard]] auto scheduler() noexcept {
  return detail::scheduler_awaiter{};
}

template<typename Duration>
[[nodiscard]] task<void> sleep_for(Duration &&duration) {
  auto *e = co_await scheduler();
  co_await e->schedule_after(std::forward<Duration>(duration));
}

}  // namespace ecoro::this_coro

#endif  // ECORO_THIS_CORO_HPP
