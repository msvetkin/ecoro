// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/manual_reset_event.hpp"

#include <iostream>

namespace ecoro {

namespace detail::_mre {

awaiter::awaiter(manual_reset_event &event) noexcept
    : event_(event) {
}

bool awaiter::await_ready() const noexcept {
  return event_.ready();
};

bool awaiter::await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept {
  awaiting_coroutine_ = awaiting_coroutine;

	const void* const set_state = static_cast<const void*>(&event_);

	void *old_state = event_.state_.load(std::memory_order_acquire);
	do {
		if (old_state == set_state) {
			return false;
		}

		next_ = static_cast<awaiter*>(old_state);
	} while (!event_.state_.compare_exchange_weak(
		old_state,
		static_cast<void*>(this),
		std::memory_order_release,
		std::memory_order_acquire));

	return true;
}

void awaiter::await_resume() const noexcept {
}

awaiter *awaiter::next() const {
  return next_;
}

void awaiter::resume() {
  awaiting_coroutine_.resume();
}


}  // namespace detail::_mre

manual_reset_event::manual_reset_event(bool start_set)
    : state_{start_set ? this : nullptr}
{

}

void manual_reset_event::set() noexcept {
  auto *const set_state = static_cast<void*>(this);

	auto *old_state = state_.exchange(set_state, std::memory_order_acq_rel);
	if (old_state == set_state) {
      return;
  }

  auto *current = static_cast<detail::_mre::awaiter*>(old_state);
  while (current != nullptr) {
			auto *next = current->next();
			current->resume();
			current = next;
  }
}

bool manual_reset_event::ready() const noexcept {
  return state_.load(std::memory_order_acquire) == static_cast<const void*>(this);
}

void manual_reset_event::reset() noexcept {
  auto *set_state = static_cast<void*>(this);
	state_.compare_exchange_strong(set_state, nullptr, std::memory_order_relaxed);
}

detail::_mre::awaiter manual_reset_event::operator co_await() const noexcept {
  return {*const_cast<manual_reset_event *>(this)};
}

}  // namespace ecoro
