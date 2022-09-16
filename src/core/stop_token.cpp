// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#include "ecoro/stop_token.hpp"

namespace ecoro {

namespace detail::_st {

void stop_callback_base::execute() noexcept {
  execute_(this);
}

void stop_state::request_stop() noexcept {
  std::lock_guard lock{mutex_};

  if (stop_requested_)
    return;

  stop_requested_ = true;

  for (auto &callback : callbacks_) {
    callback.execute();
  }

  callbacks_.clear();
}

bool stop_state::stop_requested() const noexcept {
  return stop_requested_;
}

bool stop_state::try_add_callback(stop_callback_base &callback) noexcept {
  std::lock_guard lock{mutex_};

  if (stop_requested_) {
    callback.execute();
    return false;
  }

  callbacks_.push_back(callback);
  return true;
}

void stop_state::remove_callback(stop_callback_base &callback) noexcept {
  std::lock_guard lock{mutex_};
  callbacks_.erase(callbacks_.iterator_to(callback));
}

}  // namespace detail::_st

stop_token::stop_token(std::shared_ptr<detail::_st::stop_state> state)
  : state_(state) {

}

bool stop_token::stop_requested() const noexcept {
  if (auto state = state_.lock()) {
      return state->stop_requested();
  }
  return false;
}

bool stop_token::stop_possible() const noexcept {
  return !state_.expired();
}


stop_source::stop_source()
    : state_(std::make_shared<detail::_st::stop_state>()) {
}

stop_source::stop_source(nostopstate_t /*nss*/) noexcept {
}

stop_token stop_source::get_token() const noexcept {
  return stop_token(state_);
}

void stop_source::request_stop() noexcept {
  if (state_)
    state_->request_stop();
}

bool stop_source::stop_requested() const noexcept {
  if (state_)
    return state_->stop_requested();

  return false;
}

}  // namespace ecoro
