#pragma once

#include "object.h"
#include "internals.h"

#include <type_traits>

namespace cpy {
namespace detail {

template <typename T>
struct type_caster
{
public:
    using instance_type = instance<T>;

    type_caster() {
        auto const& registered_types = get_internals().registered_types_;
        auto it = registered_types.find(&typeid(T));
        if (it != registered_types.end()) {
            typeinfo_ = &it->second;
        }
    }

    operator T*() { return value_; }
    operator T&() { return *value_; }

protected:
    T* value_ {nullptr};
    const type_info* typeinfo_ {nullptr};
    object tmp_;
};
}  // namespace detail
}  // namespace cpy
