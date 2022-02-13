// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/stop_token.hpp"

#include "gtest/gtest.h"

#include <atomic>
#include <thread>
#include <functional>

TEST(stop_token, initial_state) {
  ecoro::stop_source stop_source{ecoro::nostopstate};
  auto stop_token = stop_source.get_token();
  ASSERT_FALSE(stop_token.stop_requested());

  stop_source.request_stop();
  ASSERT_FALSE(stop_token.stop_requested());
}

TEST(stop_token, stop) {
  ecoro::stop_source stop_source;
  auto stop_token = stop_source.get_token();
  ASSERT_FALSE(stop_token.stop_requested());

  stop_source.request_stop();

  ASSERT_TRUE(stop_token.stop_requested());
}

TEST(stop_token, stop_callback) {
  ecoro::stop_source source;
  auto token = source.get_token();

  bool executed = false;
  ecoro::stop_callback callback{token, [&executed] { executed = true; }};

  ASSERT_FALSE(executed);

  source.request_stop();

  ASSERT_TRUE(executed);
}

TEST(stop_token, stop_callback_already_stoped) {
  ecoro::stop_source source;
  auto token = source.get_token();

  source.request_stop();

  bool executed = false;
  ecoro::stop_callback callback{token, [&executed] { executed = true; }};

  ASSERT_TRUE(executed);
}

TEST(stop_token, concurrent_callback_and_stop) {
  auto get_worker = [] (auto token) {
    std::atomic<bool> cancelled = false;

    while (!cancelled) {
      ecoro::stop_callback registration{token, [&cancelled] { cancelled = true; }};

      ecoro::stop_callback reg1{token, [] {}};
      ecoro::stop_callback reg2{token, [] {}};
      ecoro::stop_callback reg3{token, [] {}};
      ecoro::stop_callback reg4{token, [] {}};
      ecoro::stop_callback reg5{token, [] {}};
      ecoro::stop_callback reg6{token, [] {}};
      ecoro::stop_callback reg7{token, [] {}};
      ecoro::stop_callback reg8{token, [] {}};
      ecoro::stop_callback reg9{token, [] {}};
      ecoro::stop_callback reg10{token, [] {}};
      ecoro::stop_callback reg11{token, [] {}};
      ecoro::stop_callback reg12{token, [] {}};
      ecoro::stop_callback reg13{token, [] {}};
      ecoro::stop_callback reg14{token, [] {}};
      ecoro::stop_callback reg15{token, [] {}};
      ecoro::stop_callback reg16{token, [] {}};
      ecoro::stop_callback reg17{token, [] {}};

      std::this_thread::yield();
    }
  };

  for (int i = 0; i < 100; i++) {
    ecoro::stop_source source;
    std::thread waiter1{get_worker, source.get_token()};
    std::thread waiter2{get_worker, source.get_token()};
    std::thread waiter3{get_worker, source.get_token()};
    std::thread waiter4{get_worker, source.get_token()};
    std::thread canceller{[&source] { source.request_stop(); }};

    canceller.join();
    waiter1.join();
    waiter2.join();
    waiter3.join();
    waiter4.join();
  }
}

TEST(stop_token, move_token) {
  bool called = false;
  std::function<void()> f = [&called] { called = true; };

  ecoro::stop_source source;
  source.request_stop();

  ecoro::stop_token token = source.get_token();
  ecoro::stop_callback callback1(token, f);

  ASSERT_TRUE(token.stop_possible());
  ASSERT_TRUE(f != nullptr);
  ASSERT_EQ(called, true);

  called = false;
  ecoro::stop_callback callback2(std::move(token), f);
  ASSERT_TRUE(token.stop_possible());
  ASSERT_TRUE(f != nullptr);
  ASSERT_EQ(called, true);

  ecoro::stop_token sink(std::move(token));

  called = false;
  ecoro::stop_callback callback3(token, f);
  ASSERT_TRUE(f != nullptr);
  ASSERT_EQ(called, false);

  called = false;
  ecoro::stop_callback callback4(std::move(token), f);
  ASSERT_TRUE(f != nullptr);
  ASSERT_EQ(called, false);
}

TEST(stop_token, move_token_and_move_callback) {
  bool called = false;
  std::function<void()> f0 = [&called] { called = true; };
  std::function<void()> f = f0;

  ecoro::stop_source source;
  source.request_stop();

  ecoro::stop_token token = source.get_token();

  ecoro::stop_callback callback1(token, std::move(f));
  ASSERT_TRUE(token.stop_possible());
  ASSERT_EQ(called, true);

  called = false;
  f = f0;
  ecoro::stop_callback callback2(std::move(token), std::move(f));
  ASSERT_TRUE(token.stop_possible());
  ASSERT_EQ(called, true);

  ecoro::stop_token sink(std::move(token));

  called = false;
  f = f0;
  ecoro::stop_callback callback3(token, std::move(f));
  ASSERT_EQ(called, false);

  called = false;
  f = f0;
  ecoro::stop_callback callback4(std::move(token), std::move(f));
  ASSERT_EQ(called, false);
}
