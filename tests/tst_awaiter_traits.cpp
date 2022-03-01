// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/awaiter_traits.hpp"

#include "gtest/gtest.h"

TEST(awaiter_traits, non_awaiter) {
  struct non_awaitable {};

  using awaiter = ecoro::awaiter_traits<non_awaitable>;

  constexpr auto awaiter_return_type_check =
      std::is_same_v<awaiter::return_type, const ecoro::nonawaiter_t>;
  ASSERT_TRUE(awaiter_return_type_check);

  ASSERT_FALSE(awaiter::has_await_ready);
  ASSERT_FALSE(awaiter::has_await_suspend);
  ASSERT_FALSE(awaiter::has_await_resume);
  ASSERT_FALSE(awaiter::is_awaiter);
}

TEST(awaiter_traits, awaiter_resume_void) {
  using awaiter = ecoro::awaiter_traits<std::suspend_always>;

  constexpr auto awaiter_return_type_check =
      std::is_same_v<awaiter::return_type, void>;
  ASSERT_TRUE(awaiter_return_type_check);

  ASSERT_TRUE(awaiter::has_await_ready);
  ASSERT_TRUE(awaiter::has_await_suspend);
  ASSERT_TRUE(awaiter::has_await_resume);
  ASSERT_TRUE(awaiter::is_awaiter);
}

TEST(awaiter_traits, awaiter_resume_int) {
  struct awaiter_resume_int {
    bool await_ready() const noexcept;
    bool await_suspend(
        std::coroutine_handle<> awaitingCoroutine) const noexcept;
    int await_resume() const noexcept { return {}; }
  };

  using awaiter = ecoro::awaiter_traits<awaiter_resume_int>;

  constexpr auto awaiter_return_type_check =
      std::is_same_v<awaiter::return_type, int>;
  ASSERT_TRUE(awaiter_return_type_check);

  ASSERT_TRUE(awaiter::has_await_ready);
  ASSERT_TRUE(awaiter::has_await_suspend);
  ASSERT_TRUE(awaiter::has_await_resume);
  ASSERT_TRUE(awaiter::is_awaiter);
}
