#pragma once

#include "object.h"

namespace cpy {
class function : public object
{
public:
    using object::object;

    bool is_cpp_function() const {
        PyObject* ptr = ptr_;
        if (ptr == nullptr) {
            return false;
        }
        if (PyMethod_Check(ptr)) {
            ptr = PyMethod_GET_FUNCTION(ptr);
        }
        return PyCFunction_Check(ptr);
    }
};
}  // namespace cpy
