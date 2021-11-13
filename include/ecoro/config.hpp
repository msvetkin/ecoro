// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_CONFIG_HPP
#define ECORO_CONFIG_HPP

#if defined(__GNUC__) || defined(__clang__)
  #define ECORO_NOINLINE __attribute__((__noinline__))
#elif defined(_MSC_VER)
  #define ECORO_NOINLINE __declspec(noinline)
#else
  #define ECORO_NOINLINE
#endif

#endif  // ECORO_CONFIG_HPP
