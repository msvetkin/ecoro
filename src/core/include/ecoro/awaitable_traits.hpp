// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_AWAITABLE_TRAITS_HPP
#define ECORO_AWAITABLE_TRAITS_HPP

#include "ecoro/awaitable_concepts.hpp"
#include "ecoro/awaiter_traits.hpp"
#include "ecoro/coroutine.hpp"
#include "ecoro/concepts.hpp"

#include <type_traits>
#include <utility>

namespace ecoro {

namespace detail::_awaitable_traits {

inline const struct _get_awaiter_fn {
  template <typename Awaitable>
  constexpr decltype(auto) operator()(Awaitable &&awaitable) const noexcept {
    if constexpr (has_member_operator_co_await<Awaitable>) {
      return awaitable.operator co_await();
    } else if constexpr (has_free_operator_co_await<Awaitable>) {
      return operator co_await(static_cast<Awaitable&&>(awaitable));
    } else {
      return;
    }
  }
} get_awaiter {};

}  // namespace detail::_awaitable_traits

template <awaitable Awaitable>
using awaiter_type_t = decltype(
    detail::_awaitable_traits::get_awaiter(std::declval<Awaitable>()));

template<awaitable Awaitable>
using awaitable_return_type = awaiter_return_type<awaiter_type_t<Awaitable>>;

}  // namespace ecoro

#endif  // ECORO_AWAITABLE_TRAITS_HPP
