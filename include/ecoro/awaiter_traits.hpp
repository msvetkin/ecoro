// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_AWAITER_TRAITS_HPP
#define ECORO_AWAITER_TRAITS_HPP

#include "ecoro/coroutine.hpp"
#include "ecoro/detail/traits.hpp"

namespace ecoro {

namespace detail {

template <typename Awaiter>
using await_ready_expr = decltype(std::declval<Awaiter>().await_ready());

template <typename Awaiter>
using await_suspend_expr = decltype(std::declval<Awaiter>().await_suspend(
    std::declval<std::coroutine_handle<>>()));

template <typename Awaiter>
using await_resume_expr = decltype(std::declval<Awaiter>().await_resume());

template <typename Awaiter, typename = void>
struct has_await_resume {
  static constexpr bool value = false;
  using type = std::false_type;
};

template <typename Awaiter>
struct has_await_resume<
    Awaiter, std::enable_if_t<is_detected<await_resume_expr, Awaiter>::value>> {
  static constexpr bool value = true;
  using type = await_resume_expr<Awaiter>;
};

}  // namespace detail

template <typename T>
struct awaiter_traits {
  static constexpr bool has_await_ready =
      detail::is_detected<detail::await_ready_expr, T>::value;
  static constexpr bool has_await_suspend =
      detail::is_detected<detail::await_suspend_expr, T>::value;
  static constexpr bool has_await_resume = detail::has_await_resume<T>::value;

  using return_type = typename detail::has_await_resume<T>::type;
};

template <typename T>
using awaiter_traits_t = awaiter_traits<T>;

}  // namespace ecoro

#endif  // ECORO_AWAITER_TRAITS_HPP
