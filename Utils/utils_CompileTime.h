#pragma once

#include <type_traits>
#include <utility>

template<class T, class Method, class... Args>
concept has_member_function_with_signature = std::is_invocable_v<Method, T, Args...>;

template<class F, std::size_t... Is>
constexpr void for_(F &&func, std::index_sequence<Is...>) {
    (func(std::integral_constant<std::size_t, Is>{}), ...);
}

template<std::size_t N, class F>
constexpr void for_(F &&func) {
    for_(func, std::make_index_sequence<N>());
}