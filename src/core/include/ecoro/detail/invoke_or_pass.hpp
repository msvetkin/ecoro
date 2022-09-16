// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_DETAIL_INVOKE_OR_PASS_HPP
#define ECORO_DETAIL_INVOKE_OR_PASS_HPP

#include "ecoro/awaitable_traits.hpp"

#include <type_traits>
#include <utility>

namespace ecoro::detail {

template<typename T, typename... Args>
decltype(auto) invoke_or_pass(T &&awaitable, Args &&...args) {
  constexpr bool is_awaitable = ecoro::awaitable<T>;

  if constexpr (!is_awaitable && std::is_invocable_v<T, Args...>) {
    if constexpr (ecoro::awaitable<std::invoke_result_t<T, Args...>>) {
      return awaitable(std::forward<Args>(args)...);
    } else {
      static_assert(is_awaitable,
                    "T is invokable, but result is not awaitable");
    }
  } else if constexpr (!is_awaitable) {
    static_assert(is_awaitable, "T is not awaitable");
  } else {
    return std::forward<T>(awaitable);
  }
}

}  // namespace ecoro::detail

#endif  // ECORO_DETAIL_INVOKE_OR_PASS_HPP
