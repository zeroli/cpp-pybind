#pragma once

#include "object.h"
#include "cpp_function.h"

namespace cpy {
/// module is a wrapper to python module
/// which is also an object
class module : public object
{
public:
    using object::object;

    explicit module(const char* name) {
        ptr_ = Py_InitModule(name, nullptr);

        inc_ref();
    }
    ~module() = default;

    template <typename Func, typename... Extra>
    module& def(const char* name, Func&& func, Extra&&... extra)
    {
        cpp_function cppfunc(
            std::forward<Func>(func),
            name,
            std::forward<Extra>(extra)...);
        cppfunc.inc_ref();
        PyModule_AddObject(ptr(), name, func.ptr());
        return *this;
    }
};
}  // namespace cpy
