#pragma once

#include "object.h"

namespace cpy {
class int_t : public object {
public:
    using object::object;

    int_t(int value)
        : object(PyLong_FromLong((long)value), false)
    {}

    int_t(size_t value)
        : object(PyLong_FromSize_t(value), false)
    {}

    int_t(ssize_t value)
        : object(PyLong_FromSsize_t(value), false)
    {}

    operator int() const {
        return (int)PyLong_AsLong(ptr_);
    }
};
}  // namespace cpy
