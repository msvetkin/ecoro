// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "scheduler.hpp"

#include "ecoro/sync_wait.hpp"

#include <thread>

namespace ecoro::sts {

bool scheduler::timer_awaiter::await_ready() const noexcept {
  const auto now = std::chrono::steady_clock::now();
  return when < now;
}

bool scheduler::timer_awaiter::await_suspend(
    std::coroutine_handle<> awaiting_coro) noexcept {
  handle = awaiting_coro;
  scheduler_.add_timer_awaiter(this);
  return true;
}

void scheduler::timer_awaiter::await_resume() const noexcept {}

int scheduler::exec() {
  auto timer_task = process_timers();
  sync_wait(timer_task);
  return 1;
}

ecoro::task<void> scheduler::schedule_after(
    const std::chrono::nanoseconds delay) noexcept {
  co_return co_await timer_awaiter{
    *this, std::chrono::steady_clock::now() + delay};
}

void scheduler::add_timer_awaiter(timer_awaiter *timer) {
  timer_awaiters_.push_back(timer);
}

void scheduler::shutdown() {
  running_ = false;
}

task<void> scheduler::process_timers() {
  while (running_) {
    const auto now = std::chrono::steady_clock::now();
    if (timer_awaiters_.empty()) {
      std::this_thread::yield();
      continue;
    }

    auto timer_awaiters = std::exchange(timer_awaiters_, {});
    for (auto *timer_awaiter : timer_awaiters) {
      if (timer_awaiter->when <= now) {
        timer_awaiter->handle.resume();
        if (!running_)
          break;

      } else {
        timer_awaiters_.push_back(timer_awaiter);
      }
    }
  }

  co_return;
}

}  // namespace ecoro::sts
