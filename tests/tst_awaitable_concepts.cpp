// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/awaitable_concepts.hpp"

#include "gtest/gtest.h"

TEST(awaitable_concepts, non_awaitable) {
  struct awaitable {};

  constexpr auto has_member_operator_co_await =
      ecoro::has_member_operator_co_await<awaitable>;
  ASSERT_FALSE(has_member_operator_co_await);

  constexpr auto has_free_operator_co_await =
      ecoro::has_free_operator_co_await<awaitable>;
  ASSERT_FALSE(has_free_operator_co_await);

  constexpr auto has_co_await = ecoro::has_co_await<awaitable>;
  ASSERT_FALSE(has_co_await);

  constexpr auto is_awaitable = ecoro::awaitable<awaitable>;
  ASSERT_FALSE(is_awaitable);
}

TEST(awaitable_concepts, with_member_co_await) {
  struct awaitable {
    auto operator co_await() const &noexcept {
      return std::suspend_always{};
    }
  };

  constexpr auto has_member_operator_co_await =
      ecoro::has_member_operator_co_await<awaitable>;
  ASSERT_TRUE(has_member_operator_co_await);

  constexpr auto has_free_operator_co_await =
      ecoro::has_free_operator_co_await<awaitable>;
  ASSERT_FALSE(has_free_operator_co_await);

  constexpr auto has_co_await = ecoro::has_co_await<awaitable>;
  ASSERT_TRUE(has_co_await);

  constexpr auto is_awaitable = ecoro::awaitable<awaitable>;
  ASSERT_TRUE(is_awaitable);
}

TEST(awaitable_concepts, with_member_co_await_and_non_awaiter) {
  struct awaitable {
    auto operator co_await() const &noexcept {
      struct awaiter {};

      return awaiter{};
    }
  };

  constexpr auto has_member_operator_co_await =
      ecoro::has_member_operator_co_await<awaitable>;
  ASSERT_FALSE(has_member_operator_co_await);

  constexpr auto has_free_operator_co_await =
      ecoro::has_free_operator_co_await<awaitable>;
  ASSERT_FALSE(has_free_operator_co_await);

  constexpr auto has_co_await = ecoro::has_co_await<awaitable>;
  ASSERT_FALSE(has_co_await);

  constexpr auto is_awaitable = ecoro::awaitable<awaitable>;
  ASSERT_FALSE(is_awaitable);
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

TEST(awaitable_concepts, with_member_co_await_and_custom_awaiter) {
  struct awaitable {
    auto operator co_await() const &noexcept {
      return custom_awaiter{};
    }
  };

  constexpr auto has_member_operator_co_await =
      ecoro::has_member_operator_co_await<awaitable>;
  ASSERT_TRUE(has_member_operator_co_await);

  constexpr auto has_free_operator_co_await =
      ecoro::has_free_operator_co_await<awaitable>;
  ASSERT_FALSE(has_free_operator_co_await);

  constexpr auto has_co_await = ecoro::has_co_await<awaitable>;
  ASSERT_TRUE(has_co_await);

  constexpr auto is_awaitable = ecoro::awaitable<awaitable>;
  ASSERT_TRUE(is_awaitable);
}

namespace _non_member_co_await {

struct awaitable {};

auto operator co_await(awaitable) noexcept {
  return std::suspend_always{};
}

}  // namespace _non_member_co_await

TEST(awaitable_concepts, with_free_co_await) {
  using namespace _non_member_co_await;

  constexpr auto has_member_operator_co_await =
      ecoro::has_member_operator_co_await<awaitable>;
  ASSERT_FALSE(has_member_operator_co_await);

  constexpr auto has_free_operator_co_await =
      ecoro::has_free_operator_co_await<awaitable>;
  ASSERT_TRUE(has_free_operator_co_await);

  constexpr auto has_co_await = ecoro::has_co_await<awaitable>;
  ASSERT_TRUE(has_co_await);

  constexpr auto is_awaitable = ecoro::awaitable<awaitable>;
  ASSERT_TRUE(is_awaitable);
}

namespace _non_member_co_await_with_custom_awaiter {

struct awaitable {};

auto operator co_await(awaitable) noexcept {
  return custom_awaiter{};
}

}  // namespace _non_member_co_await

TEST(awaitable_concepts, with_free_co_await_and_custom_awaiter) {
  using namespace _non_member_co_await_with_custom_awaiter;

  constexpr auto has_member_operator_co_await =
      ecoro::has_member_operator_co_await<awaitable>;
  ASSERT_FALSE(has_member_operator_co_await);

  constexpr auto has_free_operator_co_await =
      ecoro::has_free_operator_co_await<awaitable>;
  ASSERT_TRUE(has_free_operator_co_await);

  constexpr auto has_co_await = ecoro::has_co_await<awaitable>;
  ASSERT_TRUE(has_co_await);

  constexpr auto is_awaitable = ecoro::awaitable<awaitable>;
  ASSERT_TRUE(is_awaitable);
}
