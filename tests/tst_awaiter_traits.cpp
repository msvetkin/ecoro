// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/awaiter_traits.hpp"

#include "gtest/gtest.h"

template<typename, typename = void>
struct can_get_awaiter_return_type : std::false_type {};

template<typename Awaiter>
struct can_get_awaiter_return_type<
    Awaiter, std::void_t<ecoro::awaiter_return_type<Awaiter>>>
    : std::true_type {};

TEST(awaiter_traits, no_awaiter_return_type) {
  struct awaiter {};

  ASSERT_FALSE(can_get_awaiter_return_type<awaiter>::value);
}

TEST(awaiter_traits, awaiter_return_void) {
  using awaiter = std::suspend_always;

  constexpr bool exists_return_type =
      can_get_awaiter_return_type<awaiter>::value;

  ASSERT_TRUE(exists_return_type);

  if constexpr (exists_return_type) {
    constexpr auto check_return_type =
        std::is_same_v<ecoro::awaiter_return_type<awaiter>, void>;
    ASSERT_TRUE(check_return_type);
  }
}

TEST(awaiter_traits, awaiter_return_int) {
  struct awaiter {
    bool await_ready() const noexcept;
    bool await_suspend(
        std::coroutine_handle<> awaitingCoroutine) const noexcept;
    int await_resume() const noexcept { return {}; }
  };

  constexpr bool exists_return_type =
      can_get_awaiter_return_type<awaiter>::value;

  ASSERT_TRUE(exists_return_type);

  if constexpr (exists_return_type) {
    constexpr auto check_return_type =
        std::is_same_v<ecoro::awaiter_return_type<awaiter>, int>;
    ASSERT_TRUE(check_return_type);
  }
}
