// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_AWAITER_CONCEPTS_HPP
#define ECORO_AWAITER_CONCEPTS_HPP

#include "ecoro/coroutine.hpp"
#include "ecoro/concepts.hpp"

#include <utility>

namespace ecoro {

namespace detail::_awaiter_concepts {

template<typename T, typename ... U>
concept is_valid_return_await_suspend =
    is_any_of<T, bool, void> || convertible_to<T, std::coroutine_handle<>>;

}  // namespace detail::_awaiter_concepts

template<typename Awaiter>
concept has_await_ready = requires(Awaiter awaiter) {
  { awaiter.await_ready() } -> convertible_to<bool>;
};

template<typename Awaiter>
concept has_await_suspend = requires(Awaiter awaiter) {
  { awaiter.await_suspend(std::declval<std::coroutine_handle<>>()) } ->
      detail::_awaiter_concepts::is_valid_return_await_suspend;
};

template<typename Awaiter>
concept has_await_resume = requires(Awaiter awaiter) {
  awaiter.await_resume();
};

template<typename Awaiter>
concept awaiter = has_await_ready<Awaiter> && has_await_suspend<Awaiter> &&
                  has_await_resume<Awaiter>;

}  // namespace ecoro

#endif  // ECORO_AWAITER_CONCEPTS_HPP
