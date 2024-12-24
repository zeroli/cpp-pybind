#pragma once

#include "object.h"

namespace cpy {
class float_t : public object {
public:
    using object::object;

    float_t(float value)
        : object(PyFloat_FromDouble((double)value), false)
    {}

    float_t(double value)
        : object(PyFloat_FromDouble(value), false)
    {}

    operator float() const {
        return (float)PyLong_AsDouble(ptr_);
    }
    operator double() const {
        return (double)PyLong_AsDouble(ptr_);
    }
};
}  // namespace cpy
