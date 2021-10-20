// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/coroutine.hpp"

#include <iostream>

struct task_int {
  struct promise_type {
    promise_type() {}

    task_int get_return_object() {
      return std::coroutine_handle<promise_type>::from_promise(*this);
    }

    std::suspend_always initial_suspend() { return {}; }

    std::suspend_always final_suspend() noexcept { return {}; }

    void unhandled_exception() {};

    void return_value(int value) { value_ = value; }

    int value_{-1};
  };

  task_int(std::coroutine_handle<promise_type> handle)
      : handle_(std::move(handle)) {}

  ~task_int() {
    if (handle_)
      handle_.destroy();
  }

  void resume() { handle_.resume(); }

  int result() const { return handle_.promise().value_; }

  std::coroutine_handle<promise_type> handle_{nullptr};
};

task_int foo(int value) {
  int res1 = value * value;
  int res2 = res1 + value;
  co_return res2;
}

int main() {
  auto task = foo(10);
  task.resume();
  return task.result();
}
