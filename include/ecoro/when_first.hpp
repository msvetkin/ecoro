// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_WHEN_FIRST_HPP
#define ECORO_WHEN_FIRST_HPP

#include "ecoro/detail/traits.hpp"
#include "ecoro/task.hpp"
#include "ecoro/when_any.hpp"

#include <optional>

namespace ecoro {

template<typename Awaitable, typename Trigger>
[[nodiscard]] task<std::optional<awaitable_return_type<Awaitable>>> when_first(
    Awaitable awaitable, Trigger trigger) {
  auto &&[index, awaitables] =
      co_await when_any(std::move(awaitable), std::move(trigger));

  if (index == 0) {
    co_return std::get<0>(std::move(awaitables)).result();
  }

  co_return std::nullopt;
}

}  // namespace ecoro

#endif  // ECORO_WHEN_FIRST_HPP
