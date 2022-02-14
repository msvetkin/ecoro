// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include <iostream>

#include "ecoro/detail/intrusive/list.hpp"

#include "gtest/gtest.h"

#include <array>
#include <algorithm>
// #include <list>

namespace {

struct Obj : ecoro::detail::intrusive::list_node<Obj> {
  // Obj(int v) noexcept : value(v) {}
  // int value;
};

template<typename ... Objs>
auto check_order(auto &list, Objs &&... objs) {
  const std::array expected_order = {&objs...};
  auto is_same = [] (const auto &it1, const auto &it2) {
    return &it1 == it2;
  };

  return std::equal(list.begin(), list.end(), expected_order.begin(), is_same);
};

}  // namespace

TEST(intrusive_list, initial_state) {
  ecoro::detail::intrusive::list<Obj> objs;
  ASSERT_TRUE(objs.empty());
  ASSERT_EQ(objs.begin(), objs.end());
}

TEST(intrusive_list, insert) {
  ecoro::detail::intrusive::list<Obj> objs;
  Obj obj1, obj2, obj3, obj4;

  auto it4 = objs.insert(objs.end(), obj4);

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj4);
  ASSERT_EQ(&*--objs.end(), &obj4);
  ASSERT_EQ(it4, objs.begin());
  ASSERT_TRUE(check_order(objs, obj4));

  auto it1 = objs.insert(objs.begin(), obj1);

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj1);
  ASSERT_EQ(&*++objs.begin(), &obj4);
  ASSERT_EQ(&*--objs.end(), &obj4);
  ASSERT_EQ(it1, objs.begin());
  ASSERT_EQ(it4, --objs.end());
  ASSERT_TRUE(check_order(objs, obj1, obj4));

  auto it3 = objs.insert(it4, obj3);

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj1);
  ASSERT_EQ(&*++objs.begin(), &obj3);
  ASSERT_EQ(&*--objs.end(), &obj4);
  ASSERT_EQ(it1, objs.begin());
  ASSERT_EQ(it3, ++objs.begin());
  ASSERT_EQ(it4, --objs.end());
  ASSERT_TRUE(check_order(objs, obj1, obj3, obj4));

  auto it2 = objs.insert(it3, obj2);

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj1);
  ASSERT_EQ(&*++objs.begin(), &obj2);
  ASSERT_EQ(&*--objs.end(), &obj4);
  ASSERT_EQ(it1, objs.begin());
  ASSERT_EQ(it2, ++objs.begin());
  ASSERT_EQ(it4, --objs.end());
  ASSERT_TRUE(check_order(objs, obj1, obj2, obj3, obj4));
}

TEST(intrusive_list, push_back) {
  ecoro::detail::intrusive::list<Obj> objs;
  Obj obj1, obj2, obj3, obj4;

  objs.push_back(obj1);

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj1);
  ASSERT_EQ(&*--objs.end(), &obj1);
  ASSERT_TRUE(check_order(objs, obj1));

  objs.push_back(obj2);

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj1);
  ASSERT_EQ(&*++objs.begin(), &obj2);
  ASSERT_EQ(&*--objs.end(), &obj2);
  ASSERT_TRUE(check_order(objs, obj1, obj2));

  objs.push_back(obj3);

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj1);
  ASSERT_EQ(&*++objs.begin(), &obj2);
  ASSERT_EQ(&*--objs.end(), &obj3);
  ASSERT_TRUE(check_order(objs, obj1, obj2, obj3));

  objs.push_back(obj4);

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj1);
  ASSERT_EQ(&*++objs.begin(), &obj2);
  ASSERT_EQ(&*--objs.end(), &obj4);
  ASSERT_TRUE(check_order(objs, obj1, obj2, obj3, obj4));
}

TEST(intrusive_list, erase) {
  ecoro::detail::intrusive::list<Obj> objs;
  Obj obj1, obj2, obj3, obj4;

  objs.push_back(obj1);
  objs.push_back(obj2);
  objs.push_back(obj3);
  objs.push_back(obj4);

  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_TRUE(check_order(objs, obj1, obj2, obj3, obj4));

  auto it = objs.erase(objs.begin());

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj2);
  ASSERT_EQ(&*++objs.begin(), &obj3);
  ASSERT_EQ(&*--objs.end(), &obj4);
  ASSERT_EQ(it, objs.begin());
  ASSERT_TRUE(check_order(objs, obj2, obj3, obj4));

  it = objs.erase(objs.begin());

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj3);
  ASSERT_EQ(&*++objs.begin(), &obj4);
  ASSERT_EQ(&*--objs.end(), &obj4);
  ASSERT_EQ(it, objs.begin());
  ASSERT_TRUE(check_order(objs, obj3, obj4));

  it = objs.erase(objs.begin());

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj4);
  ASSERT_EQ(&*--objs.end(), &obj4);
  ASSERT_EQ(it, objs.begin());
  ASSERT_TRUE(check_order(objs, obj4));

  objs.erase(objs.begin());

  ASSERT_TRUE(objs.empty());
  ASSERT_EQ(objs.begin(), objs.end());

  it = objs.insert(objs.begin(), obj3);

  ASSERT_FALSE(objs.empty());
  ASSERT_NE(objs.begin(), objs.end());
  ASSERT_EQ(&*objs.begin(), &obj3);
  ASSERT_EQ(&*--objs.end(), &obj3);
  ASSERT_EQ(it, objs.begin());
  ASSERT_TRUE(check_order(objs, obj3));

  objs.erase(it);

  ASSERT_TRUE(objs.empty());
  ASSERT_EQ(objs.begin(), objs.end());
}
