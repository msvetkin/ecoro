// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/scope_guard.hpp"
#include "ecoro/sync_wait.hpp"
#include "ecoro/when_any.hpp"

#include "gtest/gtest.h"

#include <vector>
#include <string_view>

TEST(when_any, sanity_check) {
  auto t1 = []() -> ecoro::task<int> {
    co_return 10;
  };

  auto t2 = []() -> ecoro::task<char> {
    co_return 'e';
  };

  auto t = ecoro::when_any(t1(), t2());
  auto [index, awaitables] = sync_wait(t);
  EXPECT_EQ(index, 0);
  EXPECT_EQ(std::get<0>(awaitables).result(), 10);

  static_assert(
      std::is_same_v<decltype(std::get<0>(awaitables).result()), int>);
  static_assert(
      std::is_same_v<decltype(std::get<1>(awaitables).result()), char>);
}

TEST(when_any, execution_order_first_finish) {
  using namespace std::string_view_literals;

  ecoro::sync_wait([]() -> ecoro::task<void> {
    std::vector<std::string_view> steps;

    auto task1 = [&steps]() -> ecoro::task<void> {
      steps.push_back("task1 started"sv);
      ecoro::scope_guard guard{[&steps] {
        steps.push_back("task1 finished"sv);
      }};
      co_return;
    };

    auto task2 = [&steps]() -> ecoro::task<int> {
      steps.push_back("task2 started"sv);
      ecoro::scope_guard guard{[&steps] {
        steps.push_back("task2 finished"sv);
      }};
      co_return 10;
    };

    auto t = ecoro::when_any(task1(), task2());
    EXPECT_TRUE(steps.empty());

    auto res = co_await t;

    EXPECT_EQ(steps, (std::vector{"task1 started"sv, "task1 finished"sv}));
    EXPECT_EQ(res.index, 0);

    co_return;
  });
}

TEST(when_any, second_finish) {
  using namespace std::string_view_literals;

  ecoro::sync_wait([]() -> ecoro::task<void> {
    std::vector<std::string_view> steps;

    auto task1 = [&steps]() -> ecoro::task<void> {
      steps.push_back("task1 started"sv);
      co_await std::suspend_always{};
      ecoro::scope_guard guard{[&steps] {
        steps.push_back("task1 finished"sv);
      }};
      co_return;
    };

    auto task2 = [&steps]() -> ecoro::task<int> {
      steps.push_back("task2 started"sv);
      ecoro::scope_guard guard{[&steps] {
        steps.push_back("task2 finished"sv);
      }};
      co_return 10;
    };

    auto t = ecoro::when_any(task1(), task2());
    EXPECT_TRUE(steps.empty());

    auto res = co_await t;

    EXPECT_EQ(steps, (std::vector{"task1 started"sv, "task2 started"sv,
                                  "task2 finished"sv}));
    EXPECT_EQ(res.index, 1);
    EXPECT_EQ(std::get<1>(res.awaitables).result(), 10);

    co_return;
  });
}
