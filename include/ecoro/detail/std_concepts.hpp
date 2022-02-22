// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_DETAIL_STD_CONCEPTS_HPP
#define ECORO_DETAIL_STD_CONCEPTS_HPP


#if __has_include(<concepts>)
#  include <concepts>

namespace ecoro {

template<typename Fn, typename ... Args>
concept invocable = std::invocable<Fn, Args...>;

template<typename Fn, typename ... Args>
concept constructible_from = std::constructible_from<Fn, Args...>;

}  // namespace ecoro

#else

#include <type_traits>
#include <utility>
#include <functional>

namespace ecoro {

template <typename T>
concept destructible = std::is_nothrow_destructible_v<T>;

template <typename T, typename ... Args>
concept constructible_from =
  destructible<T> && std::is_constructible_v<T, Args...>;

template<typename Fn, typename ... Args>
concept invocable = requires(Fn &&fn, Args &&... args) {
  std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
};

}  // namespace ecoro

#endif  // __has_include(<concepts>)

#endif  // ECORO_DETAIL_STD_CONCEPTS_HPP
