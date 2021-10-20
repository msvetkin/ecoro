// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_TASK_PROMISE_HPP
#define ECORO_TASK_PROMISE_HPP

#include "ecoro/detail/task_promise_impl.hpp"

#if !defined(SYMMETRIC_TRANSFER)
#  include <atomic>
#endif

namespace ecoro {

class executor;

template <typename T>
class task_promise : public detail::task_promise_impl<T> {
  struct final_awaiter {
    bool await_ready() const noexcept { return false; }

#ifdef SYMMETRIC_TRANSFER
    template <typename Promise>
    std::coroutine_handle<> await_suspend(
        std::coroutine_handle<Promise> coro) noexcept {
      if (coro.promise().continuation_) {
        return coro.promise().continuation_;
      }

      return std::noop_coroutine();
    }
#else
    template <typename Promise>
    void await_suspend(std::coroutine_handle<Promise> coroutine) noexcept {
      auto &promise = coroutine.promise();
      // The solution was found here https://github.com/lewissbaker/cppcoro
      // Use 'release' memory semantics in case we finish before the
      // awaiter can suspend so that the awaiting thread sees our
      // writes to the resulting value.
      // Use 'acquire' memory semantics in case the caller registered
      // the continuation before we finished. Ensure we see their write
      // to m_continuation.
      if (promise.state_.exchange(true, std::memory_order_acq_rel)) {
        promise.continuation_.resume();
      }
    }
#endif  // SYMMETRIC_TRANSFER

    void await_resume() noexcept {}
  };

 public:
  std::suspend_always initial_suspend() { return {}; }

  final_awaiter final_suspend() noexcept { return {}; }

  ecoro::executor *executor() noexcept { return executor_; }
  void set_executor(ecoro::executor *executor) noexcept {
    if (executor_ != executor)
      executor_ = executor;
  }

#if defined(SYMMETRIC_TRANSFER)
  void set_continuation(std::coroutine_handle<> continuation) noexcept {
    continuation_ = continuation;
  }
#else
  bool set_continuation(std::coroutine_handle<> continuation) noexcept {
    continuation_ = continuation;
    return !state_.exchange(true, std::memory_order_acq_rel);
  }

 private:
  std::atomic<bool> state_{false};
#endif  // SYMMETRIC_TRANSFER

 protected:
  std::coroutine_handle<> continuation_;

 private:
  ecoro::executor *executor_{nullptr};
};

}  // namespace ecoro

#endif  // ECORO_TASK_PROMISE_HPP
