// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_EXECUTOR_HPP
#define ECORO_EXECUTOR_HPP

#include "ecoro/task.hpp"

#include <chrono>

namespace ecoro {

class executor {
 public:
  [[nodiscard]] virtual task<void> sleep_for(
      const std::chrono::seconds seconds) noexcept = 0;
};

}  // namespace ecoro

#endif  // ECORO_EXECUTOR_HPP
