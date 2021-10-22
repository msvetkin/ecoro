// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/scope_guard.hpp"
#include "ecoro/sync_wait.hpp"
#include "ecoro/when_all.hpp"
#include "gtest/gtest.h"

#include <string>

TEST(when_all, sanity_check) {
  ecoro::sync_wait([]() -> ecoro::task<void> {
    bool started1 = false, finished1 = false, started2 = false,
         finished2 = false;

    auto task1 = [&started1, &finished1]() -> ecoro::task<void> {
      started1 = true;
      ecoro::scope_guard guard{[&finished1] {
        finished1 = true;
      }};
      co_return;
    };

    auto task2 = [&started2, &finished2]() -> ecoro::task<void> {
      started2 = true;
      ecoro::scope_guard guard{[&finished2] {
        finished2 = true;
      }};
      co_return;
    };

    auto t = ecoro::when_all(task1(), task2());
    EXPECT_FALSE(started1);
    EXPECT_FALSE(started2);

    co_await t;

    EXPECT_TRUE(started1);
    EXPECT_TRUE(started2);

    EXPECT_TRUE(finished1);
    EXPECT_TRUE(finished2);
  });
}

TEST(when_all, pass_as_lambda) {
  bool started = false, finished = false;

  auto task = [&started, &finished]() -> ecoro::task<void> {
    started = true;
    ecoro::scope_guard guard{[&finished] {
      finished = true;
    }};
    co_return;
  };

  auto t = ecoro::when_all(task);
  EXPECT_FALSE(started);
  EXPECT_FALSE(finished);

  ecoro::sync_wait(t);
  EXPECT_TRUE(started);
  EXPECT_TRUE(finished);
}

TEST(when_all, pass_as_awaitable) {
  bool started = false, finished = false;

  auto task = [&started, &finished]() -> ecoro::task<void> {
    started = true;
    ecoro::scope_guard guard{[&finished] {
      finished = true;
    }};
    co_return;
  };

  auto t = ecoro::when_all(task());
  EXPECT_FALSE(started);
  EXPECT_FALSE(finished);

  ecoro::sync_wait(t);
  EXPECT_TRUE(started);
  EXPECT_TRUE(finished);
}

TEST(when_all, return_result) {
  const auto res = ecoro::sync_wait([]() -> ecoro::task<int> {
    auto task1 = []() -> ecoro::task<int> {
      co_return 1;
    };

    bool finished = false;

    auto task2 = [&finished]() -> ecoro::task<void> {
      ecoro::scope_guard guard{[&finished] {
        finished = true;
      }};
      co_return;
    };

    decltype(auto) res = co_await ecoro::when_all(task1, task2);

    static_assert(
        std::is_same_v<decltype(res),
                       std::tuple<ecoro::detail::when_all_task<int>,
                                  ecoro::detail::when_all_task<void>>>);

    static_assert(std::is_same_v<decltype(std::get<0>(res).result()), int>);
    static_assert(
        std::is_same_v<decltype(std::get<1>(res).result()), std::monostate>);

    EXPECT_TRUE(finished);
    co_return std::get<0>(res).result();
  });

  EXPECT_EQ(res, 1);
}

TEST(when_all, structered_binding) {
  const auto res = ecoro::sync_wait([]() -> ecoro::task<int> {
    auto task1 = []() -> ecoro::task<int> {
      co_return 1;
    };

    auto task2 = []() -> ecoro::task<int> {
      co_return 2;
    };

    auto [r1, r2] = co_await ecoro::when_all(task1, task2);

    co_return r1.result() + r2.result();
  });

  EXPECT_EQ(res, 3);
}

TEST(when_all, exceptions) {
  const auto res = ecoro::sync_wait([]() -> ecoro::task<int> {
    auto task = [](const bool exception) -> ecoro::task<int> {
      if (exception)
        throw std::logic_error("test exception");
      else
        co_return 1;
    };

    auto t = ecoro::when_all(task(true), task(false));
    auto [r1, r2] = co_await t;
    EXPECT_THROW(r1.result(), std::logic_error);
    EXPECT_EQ(r2.result(), 1);
    co_return 1;
  });

  EXPECT_EQ(res, 1);
}

class conext_switcher {
 public:
  struct awaiter {
    bool await_ready() const noexcept {
      return false;
    };

    bool await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept {
      continuation_ = awaiting_coroutine;
      if (switcher.next_) {
        auto next = std::exchange(switcher.next_, this);
        next->continuation_.resume();
      } else {
        switcher.next_ = this;
      }

      return true;
    }

    void await_resume() const noexcept {}

    conext_switcher &switcher;
    std::coroutine_handle<> continuation_;
  };

  class context {
   public:
    explicit context(conext_switcher &switcher) : switcher_(switcher) {}

    auto operator co_await() noexcept {
      return awaiter{switcher_};
    };

    ~context() {
      if (switcher_.next_) {
        auto next = std::exchange(switcher_.next_, nullptr);
        next->continuation_.resume();
      }
    }

   private:
    conext_switcher &switcher_;
  };

  context create_context() noexcept {
    return context(*this);
  }

 private:
  awaiter *next_{nullptr};
};

TEST(when_all, switcher) {
  conext_switcher switcher;

  std::string execution_order;
  auto task = [&switcher, &execution_order]() -> ecoro::task<void> {
    auto task1 = [&]() -> ecoro::task<void> {
      auto ctx = switcher.create_context();

      ecoro::scope_guard guard{[&execution_order] {
        execution_order.push_back('4');
      }};

      execution_order.push_back('1');
      co_await ctx;
      execution_order.push_back('3');
      co_return;
    };

    auto task2 = [&switcher, &execution_order]() -> ecoro::task<void> {
      auto ctx = switcher.create_context();
      ecoro::scope_guard guard{[&execution_order] {
        execution_order.push_back('6');
      }};

      execution_order.push_back('2');
      co_await ctx;
      execution_order.push_back('5');

      co_return;
    };

    co_await ecoro::when_all(task1, task2);
    execution_order.push_back('7');
  };

  ecoro::sync_wait(task);
  EXPECT_EQ(execution_order, "1234567");
}
