// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "scheduler.hpp"
#include "signal.hpp"

#include "ecoro/task.hpp"
#include "ecoro/this_coro.hpp"

#include <iostream>

int main() {
  using namespace std::chrono_literals;

  ecoro::sts::scheduler scheduler;

  scheduler.spawn([] (auto &scheduler) -> ecoro::task<void> {
      co_await ecoro::signal_watcher<SIGINT>();
      scheduler.shutdown();
  }, scheduler);

  scheduler.spawn([] () -> ecoro::task<void> {
    auto *executor = co_await ecoro::this_coro::executor();
    while (true) {
      co_await executor->sleep_for(1s);
      std::cout << "1s\n";
    }
  });

  scheduler.spawn([] () -> ecoro::task<void> {
    while (true) {
      co_await ecoro::this_coro::sleep_for(10s);
      std::cout << "10s\n";
    }
  });

  return scheduler.exec();
}
