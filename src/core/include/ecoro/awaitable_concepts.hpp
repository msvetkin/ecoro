// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_AWAITABLE_CONCEPTS_HPP
#define ECORO_AWAITABLE_CONCEPTS_HPP

#include "ecoro/awaiter_concepts.hpp"

#include <utility>

#ifdef ECORO_WORKAROUND_MSVC_FREE_CO_AWAIT_CONCEPT
#include <type_traits>
#endif // ECORO_WORKAROUND_MSVC_FREE_CO_AWAIT_CONCEPT

namespace ecoro {

template<typename Awaitable>
concept has_member_operator_co_await = requires {
  { std::declval<Awaitable>().operator co_await() } -> awaiter;
};

#ifdef ECORO_WORKAROUND_MSVC_FREE_CO_AWAIT_CONCEPT

namespace detail::_awaitable_concepts {

template <typename Awaitable, typename = void>
constexpr bool has_free_operator_co_await = false;

template <typename Awaitable>
constexpr bool has_free_operator_co_await<
    Awaitable,
    std::void_t<decltype(operator co_await(std::declval<Awaitable>()))>> = true;

}  // namespace detail::_awaitable_concepts

template<typename Awaitable>
concept has_free_operator_co_await =
    detail::_awaitable_concepts::has_free_operator_co_await<Awaitable>;

#else

template<typename Awaitable>
concept has_free_operator_co_await = requires {
  { operator co_await(std::declval<Awaitable>()) } -> awaiter;
};

#endif // ECORO_WORKAROUND_MSVC_FREE_CO_AWAIT_CONCEPT

template<typename Awaitable>
concept has_co_await = has_member_operator_co_await<Awaitable> ||
                       has_free_operator_co_await<Awaitable>;

template<typename Awaitable>
concept awaitable = has_co_await<Awaitable>;

}  // namespace ecoro

#endif  // ECORO_AWAITABLE_CONCEPTS_HPP
