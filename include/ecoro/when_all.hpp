// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_WHEN_ALL_HPP
#define ECORO_WHEN_ALL_HPP

#include "ecoro/awaitable_traits.hpp"
#include "ecoro/detail/invoke_or_pass.hpp"
#include "ecoro/task.hpp"

#include <tuple>
#include <variant>

namespace ecoro {

namespace detail {

class when_all_counter {
 public:
  explicit when_all_counter(const std::size_t count) noexcept : count_(count) {}

  bool is_ready() const noexcept {
    return static_cast<bool>(awaiting_coroutine_);
  }

  bool try_await(std::coroutine_handle<> awaiting_coroutine) noexcept {
    awaiting_coroutine_ = awaiting_coroutine;
    return ++count_ > 1;
  }

  void notify_awaitable_completed() noexcept {
    count_--;
    if (count_ == 1) {
      awaiting_coroutine_.resume();
    }
  }

 protected:
  std::size_t count_;
  std::coroutine_handle<> awaiting_coroutine_;
};

template <typename T>
class when_all_task_promise : public task_promise<T> {
  struct final_awaiter {
    bool await_ready() const noexcept { return false; }

    template <typename Promise>
    void await_suspend(
        std::coroutine_handle<Promise> current_coroutine) noexcept {
      current_coroutine.promise().counter_->notify_awaitable_completed();
    }

    void await_resume() noexcept {}
  };

 public:
  using task_promise<T>::task_promise;
  using value_type = std::conditional_t<std::is_void_v<T>, std::monostate, T>;

  final_awaiter final_suspend() noexcept {
    return {};
  }

  value_type result() {
    if constexpr (std::is_void_v<T>) {
      return {};
    } else {
      return task_promise<T>::result();
    }
  }

  void set_counter(when_all_counter &counter) { counter_ = &counter; }

 private:
  when_all_counter *counter_{nullptr};
};

template <typename T>
class when_all_task final : public task<T, when_all_task_promise<T>> {
 public:
  using base = task<T, when_all_task_promise<T>>;
  using base::base;

  when_all_task(base &&other) noexcept
      : base(std::move(other)) {
  }

  void start(when_all_counter &counter) noexcept {
    base::handle().promise().set_counter(counter);
    base::resume();
  }
};

template <typename Awaitable>
when_all_task<awaitable_return_t<Awaitable>> make_when_all_task(
    Awaitable awaitable) {
  co_return co_await awaitable;
}

template <typename... Awaitables>
class when_all_executor {
 public:
  explicit when_all_executor(Awaitables &&...awaitables) noexcept(
      std::conjunction_v<std::is_nothrow_move_constructible<Awaitables>...>)
      : counter_(sizeof...(Awaitables)),
        awaitables_(std::forward<Awaitables>(awaitables)...) {}

  explicit when_all_executor(when_all_executor &&other) noexcept(
      std::conjunction_v<std::is_nothrow_move_constructible<Awaitables>...>)
      : counter_(sizeof...(Awaitables)),
        awaitables_(std::exchange(other.awaitables_, {})) {}

  when_all_executor &operator=(when_all_executor &&other) noexcept {
    if (std::addressof(other) != this) {
      counter_ = std::move(other.counter_);
      awaitables_ = std::move(other.awaitables_);
    }

    return *this;
  }

  auto operator co_await() const &noexcept {
    struct awaiter {
      bool await_ready() const noexcept {
        return executor_.counter_.is_ready();
      }

      bool await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept {
        return executor_.start(awaiting_coroutine);
      }

      std::tuple<Awaitables...> await_resume() noexcept {
        return std::move(executor_.awaitables_);
      }

      when_all_executor &executor_;
    };

    return awaiter{const_cast<when_all_executor &>(*this)};
  }

 protected:
  bool start(std::coroutine_handle<> awaiting_coroutine) noexcept {
    const bool res = counter_.try_await(awaiting_coroutine);
    start(std::index_sequence_for<Awaitables...>{});
    return res;
  }

  template <std::size_t... Is>
  void start(std::index_sequence<Is...>) noexcept {
    (std::get<Is>(awaitables_).start(counter_), ...);
  }

 private:
  when_all_counter counter_;
  std::tuple<Awaitables...> awaitables_;
};

template <typename... Awaitables>
decltype(auto) make_when_all_executor(Awaitables &&...awaitables) {
  return when_all_executor<Awaitables...>(
      std::forward<Awaitables>(awaitables)...);
}

}  // namespace detail

template <typename... Awaitables>
[[nodiscard]] decltype(auto) when_all(Awaitables &&...awaitables) {
  return detail::make_when_all_executor(detail::make_when_all_task(
      detail::invoke_or_pass(std::forward<Awaitables>(awaitables)))...);
}

}  // namespace ecoro

#endif  // ECORO_WHEN_ALL_HPP
