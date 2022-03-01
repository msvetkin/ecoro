// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_DETAIL_SYNC_WAIT_TASK_HPP
#define ECORO_DETAIL_SYNC_WAIT_TASK_HPP

#include "ecoro/awaitable_traits.hpp"
#include "ecoro/task.hpp"

#include <condition_variable>
#include <mutex>

namespace ecoro::detail {

class sync_wait_event {
 public:
  sync_wait_event() {}
  sync_wait_event(sync_wait_event &) = delete;
  sync_wait_event(sync_wait_event &&) = delete;
  sync_wait_event &operator=(sync_wait_event &) = delete;
  sync_wait_event &operator=(sync_wait_event &&) = delete;

  void wait() noexcept {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return is_set_; });
  }

  void release() noexcept {
    std::scoped_lock lock(mutex_);
    is_set_ = true;
    cv_.notify_all();
  }

 private:
  std::mutex mutex_;
  std::condition_variable cv_;
  bool is_set_{false};
};

template<typename T>
class sync_wait_task_promise : public task_promise<T> {
  struct final_awaiter {
    bool await_ready() const noexcept {
      return false;
    }

    template<typename Promise>
    void await_suspend(std::coroutine_handle<Promise> coroutine) noexcept {
      coroutine.promise().event_->release();
    }

    void await_resume() noexcept {}
  };

 public:
  using task_promise<T>::task_promise;

  final_awaiter final_suspend() noexcept {
    return {};
  }

  void set_event(sync_wait_event *event) noexcept {
    event_ = event;
  }

 private:
  sync_wait_event *event_{nullptr};
};

template<typename T>
class sync_wait_task final : public task<T, sync_wait_task_promise<T>> {
 public:
  using base = task<T, sync_wait_task_promise<T>>;
  using base::base;

  sync_wait_task(base &&other) noexcept
      : base(std::move(other)) {}

  void wait() noexcept {
    base::handle().promise().set_event(&event_);
    base::resume();
    event_.wait();
  }

 private:
  sync_wait_event event_;
};

template<typename Awaitable>
sync_wait_task<awaitable_return_type<Awaitable>> make_sync_wait_task(
    Awaitable &&awaitable) {
  co_return co_await std::forward<Awaitable>(awaitable);
}

template<typename Awaitable>
decltype(auto) sync_wait_impl(Awaitable &&awaitable) {
  auto task = make_sync_wait_task(std::forward<Awaitable>(awaitable));
  task.wait();
  return task.result();
}

}  // namespace ecoro::detail

#endif  // ECORO_DETAIL_SYNC_WAIT_TASK_HPP
