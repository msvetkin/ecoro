// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_DETAIL_TASK_AWAITABLE_HPP
#define ECORO_DETAIL_TASK_AWAITABLE_HPP

#include "ecoro/coroutine.hpp"
#include "ecoro/detail/traits.hpp"

namespace ecoro {

class executor;

};

namespace ecoro::detail {

template<typename Promise>
using set_executor_expr = decltype(std::declval<Promise>().set_executor(
    static_cast<executor *>(nullptr)));

template<typename Promise, typename = void>
struct has_set_executor {
  static constexpr bool value = false;
  using type = std::false_type;
};

template<typename Promise>
struct has_set_executor<
    Promise, std::enable_if_t<is_detected<set_executor_expr, Promise>::value>> {
  static constexpr bool value = true;
  using type = set_executor_expr<Promise>;
};

template<typename Promise>
struct task_awaitable {
  bool await_ready() const noexcept {
    return !coroutine_handle_ || coroutine_handle_.done();
  }

#ifdef SYMMETRIC_TRANSFER
  template<typename P>
  auto await_suspend(std::coroutine_handle<P> awaiting_coro) noexcept {
    if constexpr (has_set_executor<P>::value) {
      coroutine_handle_.promise().set_executor(
          awaiting_coro.promise().executor());
    }

    coroutine_handle_.promise().set_continuation(awaiting_coro);
    return coroutine_handle_;
  }
#else
  template<typename P>
  bool await_suspend(std::coroutine_handle<P> awaiting_coro) noexcept {
    // The solution was found here https://github.com/lewissbaker/cppcoro
    // NOTE: We are using the bool-returning version of await_suspend() here
    // to work around a potential stack-overflow issue if a coroutine
    // awaits many synchronously-completing tasks in a loop.
    //
    // We first start the task by calling resume() and then conditionally
    // attach the continuation if it has not already completed. This allows us
    // to immediately resume the awaiting coroutine without increasing
    // the stack depth, avoiding the stack-overflow problem. However, it has
    // the down-side of requiring a std::atomic to arbitrate the race between
    // the coroutine potentially completing on another thread concurrently
    // with registering the continuation on this thread.
    //
    // We can eliminate the use of the std::atomic once we have access to
    // coroutine_handle-returning await_suspend() on both MSVC and Clang
    // as this will provide ability to suspend the awaiting coroutine and
    // resume another coroutine with a guaranteed tail-call to resume().
    if constexpr (has_set_executor<P>::value) {
      coroutine_handle_.promise().set_executor(
          awaiting_coro.promise().executor());
    }
    coroutine_handle_.resume();
    return coroutine_handle_.promise().set_continuation(awaiting_coro);
  }
#endif  // SYMMETRIC_TRANSFER

  decltype(auto) await_resume() {
    return coroutine_handle_.promise().result();
  }

  std::coroutine_handle<Promise> coroutine_handle_;
};

}  // namespace ecoro::detail

#endif  // ECORO_DETAIL_TASK_AWAITABLE_HPP
