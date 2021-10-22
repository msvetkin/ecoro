// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_AWAITABLE_TRAITS_HPP
#define ECORO_AWAITABLE_TRAITS_HPP

#include "ecoro/awaiter_traits.hpp"

namespace ecoro {

namespace detail {

template<typename Awaitable>
using co_await_expr = decltype(std::declval<Awaitable>().operator co_await());

template<typename Awaitable, typename = void>
struct has_co_await {
  static constexpr bool value = false;
  using type = std::false_type;
};

template<typename Awaitable>
struct has_co_await<
    Awaitable, std::enable_if_t<is_detected<co_await_expr, Awaitable>::value>> {
  static constexpr bool value = true;
  using type = co_await_expr<Awaitable>;
};

}  // namespace detail

template<typename T>
struct awaitable_traits {
  using awaiter_type = typename detail::has_co_await<T>::type;
  using awaiter = awaiter_traits<awaiter_type>;

  static constexpr bool has_co_await = detail::has_co_await<T>::value;

  static constexpr bool is_awaitable =
      has_co_await && awaiter::has_await_ready && awaiter::has_await_suspend &&
      awaiter::has_await_resume;
};

template<typename T>
using awaitable_traits_t = awaitable_traits<T>;

template<typename T>
using awaitable_return_t = typename awaitable_traits<T>::awaiter::return_type;

}  // namespace ecoro

#endif  // ECORO_AWAITABLE_TRAITS_HPP
