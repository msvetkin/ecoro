// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_COROUTINE_HPP
#define ECORO_COROUTINE_HPP

#if __has_include(<coroutine>)
#  include <coroutine>
#elif __has_include(<experimental/coroutine>)
#  include <experimental/coroutine>

namespace std {

template<typename Promise = void>
using coroutine_handle = experimental::coroutine_handle<Promise>;

template<typename R, typename... Args>
using coroutine_traits = experimental::coroutine_traits<R, Args...>;

inline constexpr auto noop_coroutine = experimental::noop_coroutine;
using noop_coroutine_promise = experimental::noop_coroutine_promise;
using noop_coroutine_handle = experimental::noop_coroutine_handle;

using suspend_always = experimental::suspend_always;
using suspend_never = experimental::suspend_never;

}  // namespace std
#else
#  error "Your compiler does not support coroutine"
#endif

#endif  // ECORO_COROUTINE_HPP
