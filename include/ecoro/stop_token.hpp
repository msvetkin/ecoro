// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_STOP_TOKEN_HPP
#define ECORO_STOP_TOKEN_HPP

#include <memory>
#include <utility>
#include <mutex>
#include <vector>
#include <concepts>

namespace ecoro {

namespace detail::_st {

struct stop_callback_base {
  using execute_fn = void(stop_callback_base*) noexcept;
  execute_fn *execute_;

  void execute() noexcept;
};

class stop_state {
 public:
  void request_stop() noexcept;
  [[nodiscard]] bool stop_requested() const noexcept;

  bool try_add_callback(stop_callback_base *callback) noexcept;
  void remove_callback(stop_callback_base *callback) noexcept;

 private:
  bool stop_requested_{false};
  std::vector<stop_callback_base *> callbacks_;
  std::mutex mutex_;
};

}  // namespace detail::_st

class stop_token {
 public:
  stop_token() noexcept = default;
  explicit stop_token(std::shared_ptr<detail::_st::stop_state> state);

  stop_token(const stop_token &other) noexcept = default;
  stop_token& operator=(const stop_token &other) noexcept = default;

  stop_token(stop_token &&other) noexcept = default;
  stop_token& operator=(stop_token &&other) noexcept = default;

  [[nodiscard]] bool stop_requested() const noexcept;
  [[nodiscard]] bool stop_possible() const noexcept;

 private:
  template<typename Callback>
  friend class stop_callback;

  std::weak_ptr<detail::_st::stop_state> state_;
};

struct nostopstate_t {
  explicit nostopstate_t() = default;
};

inline constexpr nostopstate_t nostopstate{};

class stop_source {
 public:
  stop_source();
  explicit stop_source(nostopstate_t nss) noexcept;

  stop_source(const stop_source &other) noexcept = default;
  stop_source(stop_source &&other) noexcept = default;

  stop_source& operator=(const stop_source& other) noexcept = default;
  stop_source& operator=(stop_source&& other) noexcept = default;

  [[nodiscard]] stop_token get_token() const noexcept;
  void request_stop() noexcept;
  [[nodiscard]] bool stop_requested() const noexcept;

 private:
  std::shared_ptr<detail::_st::stop_state> state_;
};

template<typename Callback>
class [[nodiscard]] stop_callback {
  static_assert(std::is_nothrow_destructible_v<Callback>);
  static_assert(std::invocable<Callback>);

 public:
  using callback_type = Callback;

  template<typename OtherCallback>
      requires std::invocable<OtherCallback> &&
               std::constructible_from<Callback, OtherCallback>
  explicit stop_callback(const stop_token &token, OtherCallback &&callback) noexcept(
      std::is_nothrow_constructible_v<Callback, OtherCallback>)
      : model_(std::forward<OtherCallback>(callback)) {
    register_callback(token);
  }

  template<typename OtherCallback>
      requires std::invocable<OtherCallback> &&
               std::constructible_from<Callback, OtherCallback>
  explicit stop_callback(stop_token &&token, OtherCallback &&callback) noexcept(
      std::is_nothrow_constructible_v<Callback, OtherCallback>)
      : model_(std::forward<OtherCallback>(callback)) {
    register_callback(token);
  }

  ~stop_callback() {
    if (auto state = state_.lock()) {
      state->remove_callback(&model_);
    }
  }

  stop_callback(const stop_callback&) = delete;
  stop_callback& operator=(const stop_callback&) = delete;
  stop_callback(stop_callback&&) = delete;
  stop_callback& operator=(stop_callback&&) = delete;

private:

  inline void register_callback(const stop_token &token) noexcept {
    if (auto state = token.state_.lock()) {
      if (state->try_add_callback(&model_)) {
          state_ = state;
      }
    }
  }

  struct model : detail::_st::stop_callback_base {
    template<typename OtherCallback>
    explicit model(OtherCallback &&callback) noexcept
        : detail::_st::stop_callback_base{&execute},
        callback_(std::forward<OtherCallback>(callback)) {
    }

    callback_type callback_;

    static void execute(detail::_st::stop_callback_base *that) noexcept {
      static_cast<model*>(that)->callback_();
    }
  };

  model model_;
  std::weak_ptr<detail::_st::stop_state> state_;
};

template<typename Callback>
stop_callback(stop_token, Callback) -> stop_callback<Callback>;


}  // namespace ecoro

#endif  // ECORO_STOP_TOKEN_HPP
