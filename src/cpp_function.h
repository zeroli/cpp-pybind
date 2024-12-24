#pragma once

#include "function.h"

namespace cpy {
class cpp_function : public function
{
public:
    using function::function;

    template <typename R, typename... Args, typename... Extra>
    cpp_function(R (*f)(Args...), Extra&&... extra)
    {
        // TODO
    }

    // non-const method
    template <typename R, typename C, typename... Args, typename... Extra>
    cpp_function(R (C::*f)(Args...), Extra&&... extra)
    {
        // TODO
    }

    // const method
    template <typename R, typename C, typename... Args, typename... Extra>
    cpp_function(R (C::*f)(Args...) const, Extra&&... args)
    {

    }

    const char *name() const { return "TODO"; }
};
}  // namespace cpy
