// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_TASK_HPP
#define ECORO_TASK_HPP

#include "ecoro/detail/task_awaitable.hpp"
#include "ecoro/task_promise.hpp"

namespace ecoro {

template<typename T, typename Promise = task_promise<T>,
         template<typename> class Awaiter = detail::task_awaitable>
class task {
 public:
  struct promise_type : Promise {
    using Promise::Promise;

    auto get_return_object() noexcept {
      return task{std::coroutine_handle<promise_type>::from_promise(*this)};
    }
  };

  using value_type = typename promise_type::value_type;
  using handle_type = std::coroutine_handle<promise_type>;

  task() noexcept {}

  explicit task(handle_type coroutine_frame) noexcept
      : handle_(coroutine_frame) {}

  task(task &&that) noexcept
      : handle_(std::exchange(that.handle_, nullptr)) {}

  task &operator=(task &&other) noexcept {
    if (std::addressof(other) != this) {
      clear();
      handle_ = std::exchange(other.handle_, {});
    }

    return *this;
  }

  task(const task &) = delete;
  task &operator=(const task &) = delete;

  virtual ~task() {
    clear();
  }

  handle_type &handle() noexcept {
    return handle_;
  }

  void set_scheduler(scheduler *const scheduler) noexcept {
    if (handle_) {
      handle_.promise().set_scheduler(scheduler);
    }
  }

  void clear() {
    if (handle_) {
      handle_.destroy();
      handle_ = nullptr;
    }
  }

  void resume() {
    if (handle_) {
      handle_.resume();
    }
  }

  bool done() const {
    if (handle_) {
      return handle_.done();
    }

    return true;
  }

  value_type result() {
    return handle_.promise().result();
  }

  operator bool() const {
    return handle_ && !handle_.done();
  }

  auto operator co_await() const &noexcept {
    return Awaiter<promise_type>{handle_};
  }

 private:
  handle_type handle_;
};

}  // namespace ecoro

#endif  // ECORO_TASK_HPP
