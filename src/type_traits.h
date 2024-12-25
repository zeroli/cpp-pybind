#pragma once

namespace cpy {
namespace detail {
/// given a class member method signature, remove its class
/// so that type will be a freestanding function with same return
/// and same function arguments
template <typename T>
struct remove_class;

template <typename R, typename C, typename... A>
struct remove_class<R (C::*)(A...)> {
    using type = R (*)(A...);
};

template <typename R, typename C, typename... A>
struct remove_class<R (C::*)(A...) const> {
    using type = R (*)(A...);
};

template <typename R, typename C, typename... A>
using remove_class_t = typename remove_class<R, C, A...>::type;

struct void_t { };

}  // namespace detail
}  // namespace cpy
