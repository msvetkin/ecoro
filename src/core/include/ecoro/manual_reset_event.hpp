// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_MANUAL_RESET_EVENT_HPP
#define ECORO_MANUAL_RESET_EVENT_HPP

#include "ecoro/coroutine.hpp"

#include <atomic>

namespace ecoro {

class manual_reset_event;

namespace detail::_mre {

class awaiter {
 public:
  awaiter(manual_reset_event& event) noexcept;

  bool await_ready() const noexcept;

  bool await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept;

  void await_resume() const noexcept;

  awaiter *next() const;

  void resume();

 private:
  manual_reset_event& event_;
  awaiter *next_{nullptr};
  std::coroutine_handle<> awaiting_coroutine_;
};

}  // namespace detail::_mre

class manual_reset_event {
 public:
  explicit manual_reset_event(bool start_set = false);

  void set() noexcept;

  bool ready() const noexcept;

  void reset() noexcept;

  [[nodiscard]] detail::_mre::awaiter operator co_await() const noexcept;

 private:
  friend class detail::_mre::awaiter;

  std::atomic<void *> state_{};
};

}  // namespace ecoro::this_coro

#endif  // ECORO_MANUAL_RESET_EVENT_HPP
