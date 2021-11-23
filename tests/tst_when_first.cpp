// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/scope_guard.hpp"
#include "ecoro/sync_wait.hpp"
#include "ecoro/when_first.hpp"

#include "gtest/gtest.h"

TEST(when_first, sanity_check) {
  auto t1 = []() -> ecoro::task<int> {
    co_return 10;
  };

  auto t2 = []() -> ecoro::task<int> {
    co_return 11;
  };

  auto res = ecoro::sync_wait(ecoro::when_first(t1(), t2()));
  static_assert(std::is_same_v<decltype(res), std::optional<int>>);

  EXPECT_TRUE(res.has_value());
  EXPECT_EQ(res, 10);

}

TEST(when_first, execution_order_first_finish) {
  using namespace std::string_view_literals;

  ecoro::sync_wait([]() -> ecoro::task<void> {
    std::vector<std::string_view> steps;

    auto task1 = [&steps]() -> ecoro::task<int> {
      steps.push_back("task1 started"sv);
      ecoro::scope_guard guard{[&steps] {
        steps.push_back("task1 finished"sv);
      }};
      co_return 12;
    };

    auto task2 = [&steps]() -> ecoro::task<int> {
      steps.push_back("task2 started"sv);
      ecoro::scope_guard guard{[&steps] {
        steps.push_back("task2 finished"sv);
      }};
      co_return 10;
    };

    auto t = ecoro::when_first(task1(), task2());
    EXPECT_TRUE(steps.empty());

    auto res = co_await t;

    EXPECT_EQ(steps, (std::vector{"task1 started"sv, "task1 finished"sv}));
    EXPECT_TRUE(res.has_value());
    EXPECT_EQ(res, 12);

    co_return;
  });
}

TEST(when_first, second_finish) {
  using namespace std::string_view_literals;

  ecoro::sync_wait([]() -> ecoro::task<void> {
    std::vector<std::string_view> steps;

    auto task1 = [&steps]() -> ecoro::task<int> {
      steps.push_back("task1 started"sv);
      co_await std::suspend_always{};
      ecoro::scope_guard guard{[&steps] {
        steps.push_back("task1 finished"sv);
      }};
      co_return 12;
    };

    auto task2 = [&steps]() -> ecoro::task<int> {
      steps.push_back("task2 started"sv);
      ecoro::scope_guard guard{[&steps] {
        steps.push_back("task2 finished"sv);
      }};
      co_return 10;
    };

    auto t = ecoro::when_first(task1(), task2());
    EXPECT_TRUE(steps.empty());

    auto res = co_await t;

    EXPECT_EQ(steps, (std::vector{"task1 started"sv, "task2 started"sv,
                                  "task2 finished"sv}));
    EXPECT_FALSE(res.has_value());

    co_return;
  });
}
