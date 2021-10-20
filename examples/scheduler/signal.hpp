// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_EXAMPLES_SCHEDULER_SIGNAL_HPP_
#define ECORO_EXAMPLES_SCHEDULER_SIGNAL_HPP_

#include "ecoro/coroutine.hpp"

#include <signal.h>

namespace ecoro {

template<int Signal>
class signal_watcher {
 public:
  signal_watcher() noexcept {
    signal(Signal, handler);
  }

  auto operator co_await() {
    struct awaiter {
      bool await_ready() const noexcept { return false; }
      bool await_suspend(std::coroutine_handle<> awaiting_coroutine) const noexcept {
        awaiting_coroutine_ = awaiting_coroutine;
        return true;
      }
      void await_resume() const noexcept {}
    };

    return awaiter{};
  }

  static void handler(int sig) noexcept {
    if (sig == Signal && awaiting_coroutine_)
      awaiting_coroutine_.resume();
  }

 private:
  inline static std::coroutine_handle<> awaiting_coroutine_;
};

}  // namespace ecoro


#endif  // ECORO_EXAMPLES_SCHEDULER_SIGNAL_HPP_
