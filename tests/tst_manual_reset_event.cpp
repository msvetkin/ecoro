// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/manual_reset_event.hpp"
#include "ecoro/task.hpp"

#include "gtest/gtest.h"

#include <vector>
#include <string_view>

TEST(manual_reset_event, initial_state) {
  ecoro::manual_reset_event event;
  EXPECT_FALSE(event.ready());
}

TEST(manual_reset_event, initial_state_set) {
  ecoro::manual_reset_event event{true};
  EXPECT_TRUE(event.ready());
}

TEST(manual_reset_event, set_and_reset) {
  ecoro::manual_reset_event event;
  EXPECT_FALSE(event.ready());

  event.set();
  EXPECT_TRUE(event.ready());

  event.reset();
  EXPECT_FALSE(event.ready());

  event.reset();
  EXPECT_FALSE(event.ready());

  event.set();
  EXPECT_TRUE(event.ready());

  event.set();
  EXPECT_TRUE(event.ready());

  event.reset();
  EXPECT_FALSE(event.ready());
}

TEST(manual_reset_event, await_and_set) {
  using namespace std::string_view_literals;

  ecoro::manual_reset_event event;
  std::vector<std::string_view> steps;

  auto make_task1 = [&event, &steps]() -> ecoro::task<void> {
    steps.push_back("task1 started");
    co_await event;
    steps.push_back("task1 finished");
    co_return;
  };

  auto make_task2 = [&event, &steps]() -> ecoro::task<void> {
    steps.push_back("task2 started");
    event.set();
    steps.push_back("task2 finished");
    co_return;
  };

  auto t1 = make_task1();
  t1.resume();

  EXPECT_EQ(steps, (std::vector{"task1 started"sv}));

  auto t2 = make_task2();
  t2.resume();

  EXPECT_EQ(steps, (std::vector{"task1 started"sv, "task2 started"sv,
                                "task1 finished"sv, "task2 finished"sv}));
}

TEST(manual_reset_event, await_and_initial_set) {
  using namespace std::string_view_literals;

  ecoro::manual_reset_event event{true};
  std::vector<std::string_view> steps;

  auto make_task1 = [&event, &steps]() -> ecoro::task<void> {
    steps.push_back("task1 started");
    co_await event;
    steps.push_back("task1 finished");
    co_return;
  };

  auto make_task2 = [&event, &steps]() -> ecoro::task<void> {
    steps.push_back("task2 started");
    co_await event;
    steps.push_back("task2 finished");
    co_return;
  };

  auto t1 = make_task1();
  t1.resume();

  EXPECT_EQ(steps, (std::vector{"task1 started"sv, "task1 finished"sv}));

  auto t2 = make_task2();
  t2.resume();

  EXPECT_EQ(steps, (std::vector{"task1 started"sv, "task1 finished"sv,
                                "task2 started"sv, "task2 finished"sv}));
}
