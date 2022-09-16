// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_WHEN_ANY_HPP
#define ECORO_WHEN_ANY_HPP

#include "ecoro/awaitable_traits.hpp"
#include "ecoro/detail/invoke_or_pass.hpp"
#include "ecoro/task.hpp"

#include <tuple>
#include <variant>

namespace ecoro {

namespace detail {

class when_any_observer {
 public:
  explicit when_any_observer(const std::size_t awaitables_count) noexcept
      : completed_index_(awaitables_count) {}

  bool set_continuation(std::coroutine_handle<> awaiting_coroutine) noexcept {
    awaiting_coroutine_ = awaiting_coroutine;
    return true;
  }

  void on_awaitable_completed(const std::size_t index) noexcept {
    completed_index_ = index;
    if (awaiting_coroutine_) {
      awaiting_coroutine_.resume();
      return;
    }
  }

  std::size_t completed_index() const noexcept {
    return completed_index_;
  }

 private:
  std::size_t completed_index_;
  std::coroutine_handle<> awaiting_coroutine_;
};

template<std::size_t Index, typename T>
class when_any_task_promise : public task_promise<T> {
  struct final_awaiter {
    bool await_ready() const noexcept {
      return false;
    }

    template<typename Promise>
    void await_suspend(
        std::coroutine_handle<Promise> current_coroutine) noexcept {
      current_coroutine.promise().observer_->on_awaitable_completed(Index);
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

  void set_observer(when_any_observer &observer) noexcept {
    observer_ = &observer;
  }

 private:
  when_any_observer *observer_{nullptr};
};

template<std::size_t Index, typename T>
class when_any_task final : public task<T, when_any_task_promise<Index, T>> {
 public:
  using base = task<T, when_any_task_promise<Index, T>>;
  using base::base;

  when_any_task(base &&other) noexcept : base(std::move(other)) {}

  void resume(when_any_observer &observer) noexcept {
    base::handle().promise().set_observer(observer);
    base::resume();
  }
};

template<std::size_t Index, typename Awaitable>
when_any_task<Index, awaitable_return_type<Awaitable>> make_when_any_task(
    Awaitable awaitable) {
  co_return co_await awaitable;
}

template<typename... Awaitables>
class when_any_executor {
  using storage_type = std::tuple<Awaitables...>;

  struct result {
    std::size_t index{0};
    storage_type awaitables;

    result(const std::size_t i, storage_type &&awaitables)
        : index(i), awaitables(std::move(awaitables)) {}

    result(result &&) noexcept = default;
    result &operator=(result &&) noexcept = default;

    result(result &) = delete;
    result operator=(result &) = delete;
  };

  struct awaiter {
    bool await_ready() const noexcept {
      return false;
    }

    bool await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept {
      return executor_.start(awaiting_coroutine);
    }

    result await_resume() noexcept {
      return {executor_.observer_.completed_index(),
              std::move(executor_.awaitables_)};
    }

    when_any_executor &executor_;
  };

 public:
  explicit when_any_executor(Awaitables &&...awaitables) noexcept(
      std::conjunction_v<std::is_nothrow_move_constructible<Awaitables>...>)
      : awaitables_(std::forward<Awaitables>(awaitables)...) {}

  explicit when_any_executor(when_any_executor &&other) noexcept(
      std::conjunction_v<std::is_nothrow_move_constructible<Awaitables>...>)
      : observer_(std::move(other.observer_)),
        awaitables_(std::exchange(other.awaitables_, {})) {}

  when_any_executor &operator=(when_any_executor &&other) noexcept(
      std::conjunction_v<std::is_nothrow_move_constructible<Awaitables>...>) {
    if (std::addressof(other) != this) {
      observer_ = std::move(other.observer_);
      awaitables_ = std::move(other.awaitables_);
    }

    return *this;
  }

  auto operator co_await() noexcept {
    return awaiter{const_cast<when_any_executor &>(*this)};
  }

 protected:
  bool start(std::coroutine_handle<> awaiting_coroutine) noexcept {
    std::apply([this](auto &&...args) { (start(args), ...); }, awaitables_);

    if (finished()) {
      return false;
    }

    observer_.set_continuation(awaiting_coroutine);
    return true;
  }

  template<typename Awaitable>
  void start(Awaitable &awaitable) noexcept {
    if (!finished())
      awaitable.resume(observer_);
  }

  bool finished() const noexcept {
    return observer_.completed_index() < sizeof...(Awaitables);
  }

 private:
  when_any_observer observer_{sizeof...(Awaitables)};
  storage_type awaitables_;
};

template<typename... Awaitables>
decltype(auto) make_when_any_executor(Awaitables &&...awaitables) {
  return when_any_executor<Awaitables...>(
      std::forward<Awaitables>(awaitables)...);
}

template<std::size_t... Indexes, typename... Awaitables>
[[nodiscard]] decltype(auto) when_any(std::index_sequence<Indexes...>,
                                      Awaitables &&...awaitables) {
  return detail::make_when_any_executor(
      detail::make_when_any_task<Indexes, Awaitables>(
          detail::invoke_or_pass(std::forward<Awaitables>(awaitables)))...);
}

}  // namespace detail

template<typename... Awaitables>
[[nodiscard]] decltype(auto) when_any(Awaitables &&...awaitables) {
  return detail::when_any(std::index_sequence_for<Awaitables...>{},
                          std::forward<Awaitables>(awaitables)...);
}

}  // namespace ecoro

#endif  // ECORO_WHEN_ANY_HPP
