// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_SCHEDULER_HPP
#define ECORO_SCHEDULER_HPP

#include "ecoro/task.hpp"

#include <chrono>

namespace ecoro {

class scheduler {
 public:
  [[nodiscard]] virtual task<void> schedule_after(
      const std::chrono::seconds seconds) noexcept = 0;
};

}  // namespace ecoro

#endif  // ECORO_SCHEDULER_HPP
