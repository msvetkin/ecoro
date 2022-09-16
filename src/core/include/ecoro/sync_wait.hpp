// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_SYNC_WAIT_HPP
#define ECORO_SYNC_WAIT_HPP

#include "ecoro/detail/invoke_or_pass.hpp"
#include "ecoro/detail/sync_wait_task.hpp"

namespace ecoro {

template<typename Awaitable, typename... Args>
decltype(auto) sync_wait(Awaitable &&awaitable, Args &&...args) {
  return detail::sync_wait_impl(detail::invoke_or_pass(
      std::forward<Awaitable>(awaitable), std::forward<Args>(args)...));
}

}  // namespace ecoro

#endif  // ECORO_SYNC_WAIT_HPP
