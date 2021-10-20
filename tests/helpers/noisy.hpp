/* Copyright 2020 Mikhail Svetkin */

#ifndef ECORO_TESTS_HELPERS_NOISY_HPP
#define ECORO_TESTS_HELPERS_NOISY_HPP

#include <cstdint>

namespace ecoro::helpers {

struct noisy_counter {
  std::size_t ctor{0};
  std::size_t ctor_move{0};
  std::size_t ctor_copy{0};
  std::size_t assign_move{0};
  std::size_t assign_copy{0};
  std::size_t dtor{0};
};

class noisy {
 public:
  noisy(noisy_counter *const noisy_counter)
      : counter_(noisy_counter) {
    counter_->ctor++;
  }

  noisy(const noisy &other)
      : counter_(other.counter_) {
    counter_->ctor_copy++;
  }

  noisy &operator=(const noisy &other) {
    counter_ = other.counter_;
    counter_->assign_copy++;
    return *this;
  }

  noisy(noisy &&other)
      : counter_(other.counter_) {
    counter_->ctor_move++;
  }

  noisy &operator=(noisy &&other) {
    counter_ = other.counter_;
    counter_->assign_move++;
    return *this;
  }

  ~noisy() noexcept {
    counter_->dtor++;
  }

  const noisy_counter *counter() const noexcept {
    return counter_;
  }

 private:
  noisy_counter *counter_{nullptr};
};

}  // namespace ecoro::helpers

#endif  // ECORO_TESTS_HELPERS_NOISY_HPP
