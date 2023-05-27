#pragma once

#include <type_traits>
#include <utility>

namespace utils {

template <class F, std::size_t... Is>
constexpr void for_(F&& func, std::index_sequence<Is...>) {
  (func(std::integral_constant<std::size_t, Is>{}), ...);
}

template <std::size_t N, class F>
constexpr void for_(F&& func) {
  for_(func, std::make_index_sequence<N>());
}

}  // namespace utils
