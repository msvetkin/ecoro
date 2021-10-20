// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/scope.hpp"
#include "ecoro/task.hpp"

#include "gtest/gtest.h"

TEST(scope, initial_state) {
  ecoro::scope scope;
  EXPECT_EQ(scope.size(), 0);
}

TEST(scope, join_empty) {
  int step = 0;
  auto t = [] (auto &step) -> ecoro::task<void> {
    step++;
    ecoro::scope scope;
    step++;
    co_await scope.join();
    step++;
  }(step);
  EXPECT_EQ(step, 0);

  t.handle().resume();

  EXPECT_EQ(step, 3);
}

TEST(scope, join_one) {
  std::vector<int> steps;

  auto t = [] (auto &steps) -> ecoro::task<void> {
    steps.push_back(1);
    ecoro::scope scope;
    steps.push_back(2);

    scope.spawn([] (auto &steps) -> ecoro::task<void> {
      steps.push_back(3);
      co_return;
    }(steps));

    steps.push_back(4);
    co_await scope.join();
    steps.push_back(5);
  }(steps);

  EXPECT_TRUE(steps.empty());

  t.handle().resume();

  EXPECT_EQ(steps, std::vector({1, 2, 3, 4, 5}));
}
