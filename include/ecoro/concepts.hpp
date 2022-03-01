// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_CONCEPTS_HPP
#define ECORO_CONCEPTS_HPP

#include "ecoro/detail/std_concepts.hpp"

namespace ecoro {

template<typename T, typename ... U>
concept is_any_of = (std::same_as<T, U> || ...);

}  // namespace ecoro

#endif  // ECORO_CONCEPTS_HPP
