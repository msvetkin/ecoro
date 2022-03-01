// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/awaiter_concepts.hpp"
#include "ecoro/coroutine.hpp"

#include "gtest/gtest.h"

TEST(awaiter_concepts, non_awaiter) {
  struct awaiter {};

  constexpr bool has_await_ready = ecoro::has_await_ready<awaiter>;
  ASSERT_FALSE(has_await_ready);

  constexpr bool has_await_suspend = ecoro::has_await_suspend<awaiter>;
  ASSERT_FALSE(has_await_suspend);

  constexpr bool has_await_resume = ecoro::has_await_resume<awaiter>;
  ASSERT_FALSE(has_await_resume);

  constexpr bool is_awaiter = ecoro::awaiter<awaiter>;
  ASSERT_FALSE(is_awaiter);
}

TEST(awaiter_concepts, standard_awaiter) {
  using awaiter = std::suspend_always;

  constexpr bool has_await_ready = ecoro::has_await_ready<awaiter>;
  ASSERT_TRUE(has_await_ready);

  constexpr bool has_await_suspend = ecoro::has_await_suspend<awaiter>;
  ASSERT_TRUE(has_await_suspend);

  constexpr bool has_await_resume = ecoro::has_await_resume<awaiter>;
  ASSERT_TRUE(has_await_resume);

  constexpr bool is_awaiter = ecoro::awaiter<awaiter>;
  ASSERT_TRUE(is_awaiter);
}

TEST(awaiter_concepts, missing_await_ready) {
  struct custom_awaiter {
    bool await_suspend(
        std::coroutine_handle<> awaitingCoroutine) const noexcept;
    int await_resume() const noexcept { return {}; }
  };

  constexpr bool has_await_ready = ecoro::has_await_ready<custom_awaiter>;
  ASSERT_FALSE(has_await_ready);

  constexpr bool has_await_suspend = ecoro::has_await_suspend<custom_awaiter>;
  ASSERT_TRUE(has_await_suspend);

  constexpr bool has_await_resume = ecoro::has_await_resume<custom_awaiter>;
  ASSERT_TRUE(has_await_resume);

  constexpr bool is_awaiter = ecoro::awaiter<custom_awaiter>;
  ASSERT_FALSE(is_awaiter);
}

TEST(awaiter_concepts, await_ready_return_void) {
  struct custom_awaiter {
    void await_ready() const noexcept;
    bool await_suspend(
        std::coroutine_handle<> awaitingCoroutine) const noexcept;
    int await_resume() const noexcept { return {}; }
  };

  constexpr bool has_await_ready = ecoro::has_await_ready<custom_awaiter>;
  ASSERT_FALSE(has_await_ready);

  constexpr bool has_await_suspend = ecoro::has_await_suspend<custom_awaiter>;
  ASSERT_TRUE(has_await_suspend);

  constexpr bool has_await_resume = ecoro::has_await_resume<custom_awaiter>;
  ASSERT_TRUE(has_await_resume);

  constexpr bool is_awaiter = ecoro::awaiter<custom_awaiter>;
  ASSERT_FALSE(is_awaiter);
}

TEST(awaiter_concepts, missing_await_suspend) {
  struct custom_awaiter {
    bool await_ready() const noexcept;
    int await_resume() const noexcept { return {}; }
  };

  constexpr bool has_await_ready = ecoro::has_await_ready<custom_awaiter>;
  ASSERT_TRUE(has_await_ready);

  constexpr bool has_await_suspend = ecoro::has_await_suspend<custom_awaiter>;
  ASSERT_FALSE(has_await_suspend);

  constexpr bool has_await_resume = ecoro::has_await_resume<custom_awaiter>;
  ASSERT_TRUE(has_await_resume);

  constexpr bool is_awaiter = ecoro::awaiter<custom_awaiter>;
  ASSERT_FALSE(is_awaiter);
}

TEST(awaiter_concepts, await_suspend_wrong_arg) {
  struct custom_awaiter {
    bool await_ready() const noexcept;
    bool await_suspend() const noexcept;
    int await_resume() const noexcept { return {}; }
  };

  constexpr bool has_await_ready = ecoro::has_await_ready<custom_awaiter>;
  ASSERT_TRUE(has_await_ready);

  constexpr bool has_await_suspend = ecoro::has_await_suspend<custom_awaiter>;
  ASSERT_FALSE(has_await_suspend);

  constexpr bool has_await_resume = ecoro::has_await_resume<custom_awaiter>;
  ASSERT_TRUE(has_await_resume);

  constexpr bool is_awaiter = ecoro::awaiter<custom_awaiter>;
  ASSERT_FALSE(is_awaiter);
}

TEST(awaiter_concepts, await_suspend_return_int) {
  struct custom_awaiter {
    bool await_ready() const noexcept;
    int await_suspend() const noexcept;
    int await_resume() const noexcept { return {}; }
  };

  constexpr bool has_await_ready = ecoro::has_await_ready<custom_awaiter>;
  ASSERT_TRUE(has_await_ready);

  constexpr bool has_await_suspend = ecoro::has_await_suspend<custom_awaiter>;
  ASSERT_FALSE(has_await_suspend);

  constexpr bool has_await_resume = ecoro::has_await_resume<custom_awaiter>;
  ASSERT_TRUE(has_await_resume);

  constexpr bool is_awaiter = ecoro::awaiter<custom_awaiter>;
  ASSERT_FALSE(is_awaiter);
}

TEST(awaiter_concepts, missing_await_resume) {
  struct custom_awaiter {
    bool await_ready() const noexcept;
    bool await_suspend(
        std::coroutine_handle<> awaitingCoroutine) const noexcept;
  };

  constexpr bool has_await_ready = ecoro::has_await_ready<custom_awaiter>;
  ASSERT_TRUE(has_await_ready);

  constexpr bool has_await_suspend = ecoro::has_await_suspend<custom_awaiter>;
  ASSERT_TRUE(has_await_suspend);

  constexpr bool has_await_resume = ecoro::has_await_resume<custom_awaiter>;
  ASSERT_FALSE(has_await_resume);

  constexpr bool is_awaiter = ecoro::awaiter<custom_awaiter>;
  ASSERT_FALSE(is_awaiter);
}
