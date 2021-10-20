// Copyright 2021 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_DETAIL_TRAITS_HPP
#define ECORO_DETAIL_TRAITS_HPP

#include <type_traits>

namespace ecoro::detail {

struct nonesuch {
  ~nonesuch() = delete;
  nonesuch(nonesuch const&) = delete;
  void operator=(nonesuch const&) = delete;
};

template <typename Default, typename AlwaysVoid,
          template <class...> typename Op, typename... Args>
struct detector {
  using value_type = std::false_type;
  using type = Default;
};

template <typename Default, template <class...> typename Op, typename... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
  using value_type = std::true_type;
  using type = Op<Args...>;
};

template <template <class...> typename Op, typename... Args>
using is_detected =
    typename detail::detector<detail::nonesuch, void, Op, Args...>::value_type;

}  // namespace ecoro::detail

#endif  // ECORO_DETAIL_TRAITS_HPP
