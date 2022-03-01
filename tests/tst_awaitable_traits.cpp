// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/awaitable_traits.hpp"

#include "gtest/gtest.h"

template<typename, typename = void>
struct can_get_awaiter_type : std::false_type {};

template<typename Awaitable>
struct can_get_awaiter_type<
    Awaitable, std::void_t<ecoro::awaiter_type_t<Awaitable>>>
    : std::true_type {};

template<typename, typename = void>
struct can_get_awaitable_return_type : std::false_type {};

template<typename Awaitable>
struct can_get_awaitable_return_type<
    Awaitable, std::void_t<ecoro::awaitable_return_type<Awaitable>>>
    : std::true_type {};

TEST(awaitable_traits, non_awaitable) {
  struct awaitable {};

  constexpr bool exists_awaiter_type =
      can_get_awaiter_type<awaitable>::value;
  ASSERT_FALSE(exists_awaiter_type);

  constexpr bool exists_awaitable_return_type =
      can_get_awaitable_return_type<awaitable>::value;
  ASSERT_FALSE(exists_awaitable_return_type);
}

TEST(awaitable_traits, with_member_co_await) {
  struct awaitable {
    auto operator co_await() const &noexcept {
      return std::suspend_always{};
    }
  };

  constexpr bool exists_awaiter_type =
      can_get_awaiter_type<awaitable>::value;
  ASSERT_TRUE(exists_awaiter_type);

  if constexpr (exists_awaiter_type) {
    constexpr auto check_awaiter_type =
        std::is_same_v<
            ecoro::awaiter_type_t<awaitable>, std::suspend_always>;
    ASSERT_TRUE(check_awaiter_type);
  }

  constexpr bool exists_awaitable_return_type =
      can_get_awaitable_return_type<awaitable>::value;
  ASSERT_TRUE(exists_awaitable_return_type);

  if constexpr (exists_awaitable_return_type) {
    constexpr auto check_awaitable_return_type =
        std::is_same_v<
            ecoro::awaitable_return_type<awaitable>, void>;
    ASSERT_TRUE(check_awaitable_return_type);
  }
}

struct custom_awaiter {
  bool await_ready() const noexcept {
    return true;
  }

  bool await_suspend(std::coroutine_handle<>) const noexcept {
    return true;
  }

  int await_resume() const noexcept {
    return {};
  }
};

TEST(awaitable_traits, with_member_co_await_and_custom_awaiter) {
  struct awaitable {
    auto operator co_await() const &noexcept {
      return custom_awaiter{};
    }
  };

  constexpr bool exists_awaiter_type =
      can_get_awaiter_type<awaitable>::value;
  ASSERT_TRUE(exists_awaiter_type);

  if constexpr (exists_awaiter_type) {
    constexpr auto check_awaiter_type =
        std::is_same_v<
            ecoro::awaiter_type_t<awaitable>, custom_awaiter>;
    ASSERT_TRUE(check_awaiter_type);
  }

  constexpr bool exists_awaitable_return_type =
      can_get_awaitable_return_type<awaitable>::value;
  ASSERT_TRUE(exists_awaitable_return_type);

  if constexpr (exists_awaitable_return_type) {
    constexpr auto check_awaitable_return_type =
        std::is_same_v<
            ecoro::awaitable_return_type<awaitable>, int>;
    ASSERT_TRUE(check_awaitable_return_type);
  }
}

namespace _non_member_co_await {

struct awaitable {};

auto operator co_await(awaitable) noexcept {
  return std::suspend_always{};
}

}  // namespace _non_member_co_await

TEST(awaitable_traits, with_free_co_await) {
  using namespace _non_member_co_await;

  constexpr bool exists_awaiter_type =
      can_get_awaiter_type<awaitable>::value;
  ASSERT_TRUE(exists_awaiter_type);

  if constexpr (exists_awaiter_type) {
    constexpr auto check_awaiter_type =
        std::is_same_v<
            ecoro::awaiter_type_t<awaitable>, std::suspend_always>;
    ASSERT_TRUE(check_awaiter_type);
  }

  constexpr bool exists_awaitable_return_type =
      can_get_awaitable_return_type<awaitable>::value;
  ASSERT_TRUE(exists_awaitable_return_type);

  if constexpr (exists_awaitable_return_type) {
    constexpr auto check_awaitable_return_type =
        std::is_same_v<
            ecoro::awaitable_return_type<awaitable>, void>;
    ASSERT_TRUE(check_awaitable_return_type);
  }
}

namespace _non_member_co_await_with_custom_awaiter {

struct awaitable {};

auto operator co_await(awaitable) noexcept {
  return custom_awaiter{};
}

}  // namespace _non_member_co_await

TEST(awaitable_traits, with_free_co_await_and_custom_awaiter) {
  using namespace _non_member_co_await_with_custom_awaiter;

  constexpr bool exists_awaiter_type =
      can_get_awaiter_type<awaitable>::value;
  ASSERT_TRUE(exists_awaiter_type);

  if constexpr (exists_awaiter_type) {
    constexpr auto check_awaiter_type =
        std::is_same_v<
            ecoro::awaiter_type_t<awaitable>, custom_awaiter>;
    ASSERT_TRUE(check_awaiter_type);
  }

  constexpr bool exists_awaitable_return_type =
      can_get_awaitable_return_type<awaitable>::value;
  ASSERT_TRUE(exists_awaitable_return_type);

  if constexpr (exists_awaitable_return_type) {
    constexpr auto check_awaitable_return_type =
        std::is_same_v<
            ecoro::awaitable_return_type<awaitable>, int>;
    ASSERT_TRUE(check_awaitable_return_type);
  }
}
