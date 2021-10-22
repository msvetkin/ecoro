// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_SCOPE_HPP
#define ECORO_SCOPE_HPP

#include "ecoro/coroutine.hpp"
#include "ecoro/detail/invoke_or_pass.hpp"
#include "ecoro/scope_guard.hpp"

#include <atomic>

namespace ecoro {

class executor;

class scope {
 public:
  explicit scope(executor *const executor = nullptr);

  template<typename Awaitable, typename... Args>
  void spawn(Awaitable &&awaitable, Args &&...args) {
    run(detail::invoke_or_pass(std::forward<Awaitable>(awaitable),
                               std::forward<Args>(args)...));
  }

  [[nodiscard]] auto join() noexcept {
    class join_awaiter {
     public:
      explicit join_awaiter(scope &scope) noexcept
          : scope_(scope) {}

      bool await_ready() noexcept {
        return !scope_.size();
      }

      bool await_suspend(std::coroutine_handle<> continuation) noexcept {
        if (scope_.size()) {
          scope_.continuation_ = continuation;
          return false;
        }

        return true;
      }

      void await_resume() noexcept {}

     private:
      scope &scope_;
    };

    return join_awaiter{*this};
  }

  std::size_t size() const noexcept;

 private:
  struct oneway_task {
    struct promise_type {
      std::suspend_never initial_suspend() const noexcept {
        return {};
      }

      std::suspend_never final_suspend() const noexcept {
        return {};
      }

      void unhandled_exception() const {
        std::terminate();
      }

      oneway_task get_return_object() const noexcept {
        return {};
      }

      void return_void() const noexcept {}
    };
  };

  template<typename Awaitable>
  oneway_task run(Awaitable awaitable) {
    on_task_started();
    awaitable.set_executor(executor_);
    auto on_completion = scope_guard(this, &scope::on_task_finished);
    co_await std::move(awaitable);
  }

  void on_task_started();
  void on_task_finished();

 private:
  std::atomic<std::size_t> count_{0u};
  std::coroutine_handle<> continuation_;
  executor *executor_{nullptr};
};

}  // namespace ecoro

#endif  // ECORO_SCOPE_HPP
