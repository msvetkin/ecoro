// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_AWAITER_TRAITS_HPP
#define ECORO_AWAITER_TRAITS_HPP

#include "ecoro/awaiter_concepts.hpp"

#include <utility>

namespace ecoro {

template<awaiter Awaiter>
using awaiter_return_type = decltype(std::declval<Awaiter>().await_resume());

}  // namespace ecoro

#endif  // ECORO_AWAITER_TRAITS_HPP
