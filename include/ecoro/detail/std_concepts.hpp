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

template <typename From, typename To>
concept convertible_to = std::convertible_to<From, To>;

template<typename T, typename U>
concept same_as = std::same_as<T, U>;

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

template <typename From, typename To>
concept convertible_to = std::is_convertible_v<From, To> && requires {
  static_cast<To>(std::declval<From>());
};

template<typename Fn, typename ... Args>
concept invocable = requires(Fn &&fn, Args &&... args) {
  std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
};

namespace detail::_std_concepts {

template<typename T, typename U>
concept same_helper = std::is_same_v<T, U>;

}  // namespace detail

template<typename T, typename U>
concept same_as = detail::_std_concepts::same_helper<T, U> &&
                  detail::_std_concepts::same_helper<U, T>;

}  // namespace ecoro

#endif  // __has_include(<concepts>)

#endif  // ECORO_DETAIL_STD_CONCEPTS_HPP
