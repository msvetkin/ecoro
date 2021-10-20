// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/awaitable_traits.hpp"

#include "gtest/gtest.h"

TEST(awaitable_traits, non_awaitable) {
  struct non_awaitable {};

  using awaitable_traits = ecoro::awaitable_traits_t<non_awaitable>;

  constexpr auto awaiter_type_check =
      std::is_same_v<awaitable_traits::awaiter_type, std::false_type>;
  EXPECT_TRUE(awaiter_type_check);
  EXPECT_FALSE(awaitable_traits::has_co_await);
  EXPECT_FALSE(awaitable_traits::is_awaitable);
}

TEST(awaitable_traits, awaitable_return_void) {
  struct awaitable {
    auto operator co_await() const& noexcept { return std::suspend_always{}; }
  };

  using awaitable_traits = ecoro::awaitable_traits_t<awaitable>;

  constexpr auto awaiter_type_check =
      std::is_same_v<awaitable_traits::awaiter_type, std::suspend_always>;
  EXPECT_TRUE(awaiter_type_check);
  EXPECT_TRUE(awaitable_traits::has_co_await);
  EXPECT_TRUE(awaitable_traits::is_awaitable);
}

TEST(awaitable_traits, awaitable_return_int) {
  struct awaiter {
    bool await_ready() const noexcept;
    bool await_suspend(
        std::coroutine_handle<> awaitingCoroutine) const noexcept;
    int await_resume() const noexcept;
  };

  struct awaitable {
    auto operator co_await() const& noexcept { return awaiter{}; }
  };

  using awaitable_traits = ecoro::awaitable_traits_t<awaitable>;

  constexpr auto awaiter_type_check =
      std::is_same_v<awaitable_traits::awaiter_type, awaiter>;
  EXPECT_TRUE(awaiter_type_check);
  EXPECT_TRUE(awaitable_traits::has_co_await);
  EXPECT_TRUE(awaitable_traits::is_awaitable);
}

template <typename T>
auto testReturnType(T&& t) ->
    typename ecoro::awaitable_traits<T&&>::awaiter::return_type;

TEST(awaitableTraits, return_type_for_function) {
  struct awaitable {
    auto operator co_await() const& noexcept { return std::suspend_always{}; }
  };

  constexpr bool returnVoid =
      std::is_same_v<decltype(testReturnType(awaitable{})), void>;
  EXPECT_TRUE(returnVoid);
}
