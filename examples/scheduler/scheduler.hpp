// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_EXAMPLES_SCHEDULER_SCHEDULER_HPP
#define ECORO_EXAMPLES_SCHEDULER_SCHEDULER_HPP

#include "ecoro/executor.hpp"
#include "ecoro/scope.hpp"
#include "ecoro/task.hpp"

#include <vector>

namespace ecoro::sts {

class scheduler : public executor {
  struct timer_awaiter {
    bool await_ready() const noexcept;

    bool await_suspend(std::coroutine_handle<> awaiting_coro) noexcept;
    void await_resume() const noexcept;

    scheduler &scheduler_;
    const std::chrono::time_point<std::chrono::system_clock> when;
    std::coroutine_handle<> handle;
  };

 public:
  template<typename Awaitable, typename... Args>
  void spawn(Awaitable &&awaitable, Args &&...args) {
    scope_.spawn(std::forward<Awaitable>(awaitable),
                 std::forward<Args>(args)...);
  }

  void shutdown();
  int exec();

  [[nodiscard]] ecoro::task<void> sleep_for(
      const std::chrono::seconds seconds) noexcept override;

 protected:
  void add_timer_awaiter(timer_awaiter *timer);

 protected:
  task<void> process_timers();

 private:
  bool running_{true};
  std::vector<timer_awaiter *> timer_awaiters_;
  ecoro::scope scope_{this};
};

}  // namespace ecoro::sts

#endif  // ECORO_EXAMPLES_SCHEDULER_SCHEDULER_HPP
