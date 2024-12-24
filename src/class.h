#pragma once

#include "custom_type.h"
#include "cpp_function.h"

namespace cpy {
template <typename T>
class class_ : public custom_type
{
public:
    using custom_type::custom_type;

    class_(object& scope, const char* name)
        : custom_type(scope, name, &typeid(T), sizeof(T))
    {}

    class_(object& scope, const char* name, object& parent)
        : custom_type(scope, name, &typeid(T), sizeof(T), parent.ptr())
    { }

    /// define class method
    template <typename Func, typename... Extra>
    class_& def(const char* name, Func&& func, Extra&&... extra)
    {
        cpp_function cf(std::forward<Func>(func),
                                name,
                                true,  /// is_method
                                std::forward<Extra>(extra)...);
        attr(cf.name()) = cf;
        return *this;
    }

    /// define class static function
    template <typename Func, typename... Extra>
    class_& def_static(const char* name, Func&& func, Extra&&... extra)
    {
        cpp_function cf(std::forward<Func>(func),
                                name,
                                false, /// is not method
                                std::forward<Extra>(extra)...);
        attr(cf.name()) = cf;
        return *this;
    }
};
}  // namespace cpy
