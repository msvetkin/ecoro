// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/scope.hpp"

#include "ecoro/executor.hpp"

namespace ecoro {

scope::scope(executor *const executor)
    : executor_(executor) {
}

std::size_t scope::size() const noexcept {
  return count_.load(std::memory_order_acquire);
}

void scope::on_task_started() {
  count_.fetch_add(1, std::memory_order_relaxed);
}

void scope::on_task_finished() {
  if (count_.fetch_sub(1u, std::memory_order_acq_rel) == 0 && continuation_) {
    continuation_.resume();
  }
}

}  // namespace ecoro
