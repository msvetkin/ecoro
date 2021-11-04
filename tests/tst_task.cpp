// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/detail/compiler.hpp"
#include "ecoro/sync_wait.hpp"
#include "ecoro/task.hpp"
#include "gtest/gtest.h"
#include "helpers/noisy.hpp"

#include <stdexcept>
#include <type_traits>

template<class T>
using copy_assign_t = decltype(std::declval<T &>() = std::declval<const T &>());

template<typename T>
inline constexpr bool is_copy_assign_v =
    ecoro::detail::is_detected<copy_assign_t, T>::value;

template<class T>
using copyible_t = decltype(T(std::declval<const T &>()));

template<typename T>
inline constexpr bool is_copyible_v =
    ecoro::detail::is_detected<copyible_t, T>::value;

static_assert(!is_copy_assign_v<ecoro::task<void>>,
              "Task should not support asign");

TEST(task, initial_state) {
  ecoro::task<void> taskVoid;
  EXPECT_FALSE(taskVoid);

  ecoro::task<int> taskInt;
  EXPECT_FALSE(taskInt);
}

TEST(task, move_task) {
  auto task = []() -> ecoro::task<void> {
    co_return;
  }();

  EXPECT_TRUE(task);

  auto task2 = std::move(task);

  EXPECT_FALSE(task);
  EXPECT_TRUE(task2);

  task = std::move(task2);

  EXPECT_TRUE(task);
  EXPECT_FALSE(task2);

  task = std::move(task);
  EXPECT_TRUE(task);
}

TEST(task, simple_return_void) {
  auto task = []() -> ecoro::task<void> {
    co_return;
  }();

  ecoro::sync_wait(task);
}

TEST(task, simple_return_int) {
  auto task = []() -> ecoro::task<int> {
    co_return 1;
  }();

  const auto value = ecoro::sync_wait(task);
  EXPECT_EQ(value, 1);
}

TEST(task, capture_life_time_with_temporary_lambda) {
  ecoro::helpers::noisy_counter counter;
  ecoro::helpers::noisy noisy{&counter};

  auto task = [noisy = std::move(noisy)]() -> ecoro::task<void> {
    co_return;
  }();

  EXPECT_EQ(counter.ctor, 1);
  EXPECT_EQ(counter.ctor_move, 1);
  EXPECT_EQ(counter.ctor_copy, 0);
  EXPECT_EQ(counter.assign_move, 0);
  EXPECT_EQ(counter.assign_copy, 0);
  EXPECT_EQ(counter.dtor, 1);
}

TEST(task, simple_return_int_ref) {
  int value = 2;
  auto task = [](auto &value) -> ecoro::task<int &> {
    co_return value;
  }(value);

  decltype(auto) res = ecoro::sync_wait(task);
  constexpr bool return_type_check = std::is_same_v<decltype(res), int &>;
  EXPECT_TRUE(return_type_check);
  EXPECT_EQ(value, res);
}

TEST(task, simple_return_int_pointer) {
  int value = 10;
  int *value_ptr = &value;
  auto task = [](auto *value_ptr) -> ecoro::task<int *> {
    co_return value_ptr;
  }(value_ptr);

  decltype(auto) res = ecoro::sync_wait(task);
  constexpr bool return_type_check = std::is_same_v<decltype(res), int *>;
  EXPECT_TRUE(return_type_check);
  EXPECT_TRUE(res);
  EXPECT_EQ(&value, res);
  EXPECT_EQ(value, *res);
}

TEST(task, simple_return_temporary_object) {
  ecoro::helpers::noisy_counter counter;

  {
    decltype(auto) res =
        ecoro::sync_wait([&counter]() -> ecoro::task<ecoro::helpers::noisy> {
          co_return ecoro::helpers::noisy{&counter};
        });
    constexpr bool return_type_check =
        std::is_same_v<ecoro::helpers::noisy, decltype(res)>;
    EXPECT_TRUE(return_type_check);
    EXPECT_EQ(res.counter(), &counter);
  }

  EXPECT_EQ(counter.ctor, 1);
  EXPECT_EQ(counter.ctor_move, 4);
  EXPECT_EQ(counter.ctor_copy, 0);
  EXPECT_EQ(counter.assign_move, 0);
  EXPECT_EQ(counter.assign_copy, 0);
  EXPECT_EQ(counter.dtor, 5);
}

TEST(task, result_twice) {
  ecoro::helpers::noisy_counter counter;

  ecoro::sync_wait([]() -> ecoro::task<void> {
    auto task = []() -> ecoro::task<std::string> {
      co_return "test";
    }();

    auto res = co_await task;
    EXPECT_EQ(res, "test");
  });
}

TEST(Task, ReturnNonDefaultConstructable) {
  struct object {
    object(int id) : id_(id) {}

    int id_;
  };

  auto res =
      ecoro::sync_wait([]() -> ecoro::task<object> { co_return object{10}; });
  EXPECT_EQ(res.id_, 10);
}

TEST(task, nested_tasks) {
  bool executed = false;
  auto task = [&executed]() -> ecoro::task<void> {
    executed = true;
    co_return;
  };

  ecoro::sync_wait([&task, &executed]() -> ecoro::task<void> {
    EXPECT_FALSE(executed);

    co_await task();

    EXPECT_TRUE(executed);
    co_return;
  }());

  EXPECT_TRUE(executed);
}

TEST(task, loop_stack_overflow) {
  auto loop = [](const int count) -> ecoro::task<int> {
    auto one_iteration = []() -> ecoro::task<void> {
      co_return;
    };

    int i = 0;
    for (; i < count; ++i) {
      co_await one_iteration();
    }

    co_return i;
  };

  const int count = 1'000'000;
  auto value = ecoro::sync_wait(loop(count));
  EXPECT_EQ(value, count);
}

TEST(task, catch_exception_sync_wait) {
  auto throw_exception = []() -> ecoro::task<void> {
    throw std::logic_error("test exception");

    co_return;
  };

  EXPECT_THROW(ecoro::sync_wait(throw_exception), std::logic_error);
}

TEST(task, catch_exception_void) {
  auto throw_exception = []() -> ecoro::task<void> {
    throw std::logic_error("test exception");

    co_return;
  };

  ecoro::sync_wait([&throw_exception]() -> ecoro::task<void> {
    EXPECT_THROW(co_await throw_exception(), std::logic_error);
  });
}

TEST(task, catch_exception_int) {
  auto throw_exception = []() -> ecoro::task<int> {
    throw std::logic_error("test exception");

    co_return 3;
  };

  ecoro::sync_wait([&throw_exception]() -> ecoro::task<void> {
    EXPECT_THROW(co_await throw_exception(), std::logic_error);
  });
}

TEST(task, with_arg_value_explicit) {
  ecoro::helpers::noisy_counter noise_counter;
  ecoro::helpers::noisy noisy{&noise_counter};

  ecoro::sync_wait([](auto noisy) -> ecoro::task<void> {
    EXPECT_TRUE(noisy.counter());
    EXPECT_EQ(noisy.counter()->ctor, 1);
    EXPECT_EQ(noisy.counter()->ctor_move, 1);
    EXPECT_EQ(noisy.counter()->ctor_copy, 1);
    EXPECT_EQ(noisy.counter()->assign_move, 0);
    EXPECT_EQ(noisy.counter()->assign_copy, 0);
#ifdef ECORO_COMPILER_MSVC
    EXPECT_EQ(noisy.counter()->dtor, 1);
#else
    EXPECT_EQ(noisy.counter()->dtor, 0);
#endif
    co_return;
  }(noisy));

  EXPECT_TRUE(noisy.counter());
  EXPECT_EQ(noisy.counter(), &noise_counter);
  EXPECT_EQ(noisy.counter()->ctor, 1);
  EXPECT_EQ(noisy.counter()->ctor_move, 1);
  EXPECT_EQ(noisy.counter()->ctor_copy, 1);
  EXPECT_EQ(noisy.counter()->assign_move, 0);
  EXPECT_EQ(noisy.counter()->assign_copy, 0);
  EXPECT_EQ(noisy.counter()->dtor, 2);
}

TEST(task, with_arg_value_implicit) {
  ecoro::helpers::noisy_counter noise_counter;
  ecoro::helpers::noisy noisy{&noise_counter};

  ecoro::sync_wait(
      [](auto noisy) -> ecoro::task<void> {
        EXPECT_TRUE(noisy.counter());
        EXPECT_EQ(noisy.counter()->ctor, 1);
        EXPECT_EQ(noisy.counter()->ctor_move, 1);
        EXPECT_EQ(noisy.counter()->ctor_copy, 1);
        EXPECT_EQ(noisy.counter()->assign_move, 0);
        EXPECT_EQ(noisy.counter()->assign_copy, 0);
        EXPECT_EQ(noisy.counter()->dtor, 1);
        co_return;
      },
      noisy);

  EXPECT_TRUE(noisy.counter());
  EXPECT_EQ(noisy.counter(), &noise_counter);
  EXPECT_EQ(noisy.counter()->ctor, 1);
  EXPECT_EQ(noisy.counter()->ctor_move, 1);
  EXPECT_EQ(noisy.counter()->ctor_copy, 1);
  EXPECT_EQ(noisy.counter()->assign_move, 0);
  EXPECT_EQ(noisy.counter()->assign_copy, 0);
  EXPECT_EQ(noisy.counter()->dtor, 2);
}

TEST(task, with_arg_ref_explicit) {
  ecoro::helpers::noisy_counter noise_counter;
  ecoro::helpers::noisy noisy{&noise_counter};

  ecoro::sync_wait([](auto &noisy) -> ecoro::task<void> {
    EXPECT_TRUE(noisy.counter());
    EXPECT_EQ(noisy.counter()->ctor, 1);
    EXPECT_EQ(noisy.counter()->ctor_move, 0);
    EXPECT_EQ(noisy.counter()->ctor_copy, 0);
    EXPECT_EQ(noisy.counter()->assign_move, 0);
    EXPECT_EQ(noisy.counter()->assign_copy, 0);
    EXPECT_EQ(noisy.counter()->dtor, 0);
    co_return;
  }(noisy));

  EXPECT_TRUE(noisy.counter());
  EXPECT_EQ(noisy.counter(), &noise_counter);
  EXPECT_EQ(noisy.counter()->ctor, 1);
  EXPECT_EQ(noisy.counter()->ctor_move, 0);
  EXPECT_EQ(noisy.counter()->ctor_copy, 0);
  EXPECT_EQ(noisy.counter()->assign_move, 0);
  EXPECT_EQ(noisy.counter()->assign_copy, 0);
  EXPECT_EQ(noisy.counter()->dtor, 0);
}

TEST(task, with_arg_ref_implicit) {
  ecoro::helpers::noisy_counter noise_counter;
  ecoro::helpers::noisy noisy{&noise_counter};

  ecoro::sync_wait(
      [](auto &noisy) -> ecoro::task<void> {
        EXPECT_TRUE(noisy.counter());
        EXPECT_EQ(noisy.counter()->ctor, 1);
        EXPECT_EQ(noisy.counter()->ctor_move, 0);
        EXPECT_EQ(noisy.counter()->ctor_copy, 0);
        EXPECT_EQ(noisy.counter()->assign_move, 0);
        EXPECT_EQ(noisy.counter()->assign_copy, 0);
        EXPECT_EQ(noisy.counter()->dtor, 0);
        co_return;
      },
      noisy);

  EXPECT_TRUE(noisy.counter());
  EXPECT_EQ(noisy.counter(), &noise_counter);
  EXPECT_EQ(noisy.counter()->ctor, 1);
  EXPECT_EQ(noisy.counter()->ctor_move, 0);
  EXPECT_EQ(noisy.counter()->ctor_copy, 0);
  EXPECT_EQ(noisy.counter()->assign_move, 0);
  EXPECT_EQ(noisy.counter()->assign_copy, 0);
  EXPECT_EQ(noisy.counter()->dtor, 0);
}

TEST(task, with_arg_rvalue_explicit) {
  ecoro::helpers::noisy_counter noise_counter;
  ecoro::helpers::noisy noisy{&noise_counter};

  ecoro::sync_wait([](auto &&noisy) -> ecoro::task<void> {
    EXPECT_TRUE(noisy.counter());
    EXPECT_EQ(noisy.counter()->ctor, 1);
    EXPECT_EQ(noisy.counter()->ctor_move, 0);
    EXPECT_EQ(noisy.counter()->ctor_copy, 0);
    EXPECT_EQ(noisy.counter()->assign_move, 0);
    EXPECT_EQ(noisy.counter()->assign_copy, 0);
    EXPECT_EQ(noisy.counter()->dtor, 0);
    co_return;
  }(std::move(noisy)));

  EXPECT_TRUE(noisy.counter());
  EXPECT_EQ(noisy.counter(), &noise_counter);
  EXPECT_EQ(noisy.counter()->ctor, 1);
  EXPECT_EQ(noisy.counter()->ctor_move, 0);
  EXPECT_EQ(noisy.counter()->ctor_copy, 0);
  EXPECT_EQ(noisy.counter()->assign_move, 0);
  EXPECT_EQ(noisy.counter()->assign_copy, 0);
  EXPECT_EQ(noisy.counter()->dtor, 0);
}

TEST(task, with_arg_rvalue_implicit) {
  ecoro::helpers::noisy_counter noise_counter;
  ecoro::helpers::noisy noisy{&noise_counter};

  ecoro::sync_wait(
      [](auto &&noisy) -> ecoro::task<void> {
        EXPECT_TRUE(noisy.counter());
        EXPECT_EQ(noisy.counter()->ctor, 1);
        EXPECT_EQ(noisy.counter()->ctor_move, 0);
        EXPECT_EQ(noisy.counter()->ctor_copy, 0);
        EXPECT_EQ(noisy.counter()->assign_move, 0);
        EXPECT_EQ(noisy.counter()->assign_copy, 0);
        EXPECT_EQ(noisy.counter()->dtor, 0);
        co_return;
      },
      std::move(noisy));

  EXPECT_TRUE(noisy.counter());
  EXPECT_EQ(noisy.counter(), &noise_counter);
  EXPECT_EQ(noisy.counter()->ctor, 1);
  EXPECT_EQ(noisy.counter()->ctor_move, 0);
  EXPECT_EQ(noisy.counter()->ctor_copy, 0);
  EXPECT_EQ(noisy.counter()->assign_move, 0);
  EXPECT_EQ(noisy.counter()->assign_copy, 0);
  EXPECT_EQ(noisy.counter()->dtor, 0);
}
