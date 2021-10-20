// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/awaiter_traits.hpp"

#include "gtest/gtest.h"

TEST(awaiter_traits, non_awaiter) {
  struct non_awaitable {};

  using awaiter = ecoro::awaiter_traits_t<non_awaitable>;

  constexpr auto awaiter_return_type_check =
      std::is_same_v<awaiter::return_type, std::false_type>;
  EXPECT_TRUE(awaiter_return_type_check);

  EXPECT_FALSE(awaiter::has_await_ready);
  EXPECT_FALSE(awaiter::has_await_suspend);
  EXPECT_FALSE(awaiter::has_await_resume);
}

TEST(awaiter_traits, awaiter_resume_void) {
  using awaiter = ecoro::awaiter_traits_t<std::suspend_always>;

  constexpr auto awaiter_return_type_check =
      std::is_same_v<awaiter::return_type, void>;
  EXPECT_TRUE(awaiter_return_type_check);

  EXPECT_TRUE(awaiter::has_await_ready);
  EXPECT_TRUE(awaiter::has_await_suspend);
  EXPECT_TRUE(awaiter::has_await_resume);
  EXPECT_TRUE(awaiter::has_await_resume);
}

TEST(awaiter_traits, awaiter_resume_int) {
  struct awaiter_resume_int {
    bool await_ready() const noexcept;
    bool await_suspend(
        std::coroutine_handle<> awaitingCoroutine) const noexcept;
    int await_resume() const noexcept;
  };

  using awaiter = ecoro::awaiter_traits_t<awaiter_resume_int>;

  constexpr auto awaiter_return_type_check =
      std::is_same_v<awaiter::return_type, int>;
  EXPECT_TRUE(awaiter_return_type_check);

  EXPECT_TRUE(awaiter::has_await_ready);
  EXPECT_TRUE(awaiter::has_await_suspend);
  EXPECT_TRUE(awaiter::has_await_resume);
  EXPECT_TRUE(awaiter::has_await_resume);
}
