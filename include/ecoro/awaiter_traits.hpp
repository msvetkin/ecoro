// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_AWAITER_TRAITS_HPP
#define ECORO_AWAITER_TRAITS_HPP

#include "ecoro/awaiter_concepts.hpp"

#include <utility>

namespace ecoro {

struct nonawaiter_t {
  explicit nonawaiter_t() = default;
};

inline constexpr nonawaiter_t nonawaiter{};

namespace detail::_awaiter_traits {

inline const struct _get_awaiter_return_type_fn {
  template <typename Awaiter>
  constexpr decltype(auto) operator()(Awaiter &&awaiter) const noexcept {
    if constexpr (has_await_resume<Awaiter>) {
      return awaiter.await_resume();
    } else {
      return nonawaiter;
    }
  }
} get_awaiter_return_type{};

template <typename Awaiter>
using awaiter_return_type = decltype(
    get_awaiter_return_type(std::declval<Awaiter>()));

template<typename Awaiter>
struct awaiter_traits {
  static constexpr bool has_await_ready = ecoro::has_await_ready<Awaiter>;
  static constexpr bool has_await_suspend = ecoro::has_await_suspend<Awaiter>;
  static constexpr bool has_await_resume = ecoro::has_await_resume<Awaiter>;
  static constexpr bool is_awaiter = ecoro::awaiter<Awaiter>;

  using return_type = awaiter_return_type<Awaiter>;
};

}  // namespace detail::_awaiter_traits

template<awaiter Awaiter>
using awaiter_return_type =
    detail::_awaiter_traits::awaiter_return_type<Awaiter>;

template<typename Awaiter>
using awaiter_traits = detail::_awaiter_traits::awaiter_traits<Awaiter>;

}  // namespace ecoro

#endif  // ECORO_AWAITER_TRAITS_HPP
