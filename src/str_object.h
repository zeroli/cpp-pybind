#pragma once

#include "object.h"

namespace cpy {
class str_t : public object {
public:
    using object::object;

    str_t(const char* s)
        : object(PyUnicode_FromString(s), false)
    {
    }

    operator const char*() const {
        if (tmp_.ptr()) {
            return PyString_AsString(tmp_.ptr());
        }
        tmp_ = object(PyUnicode_AsUTF8String(ptr_), false);
        if (tmp_.ptr() == nullptr) {
            return nullptr;
        }
        return PyString_AsString(tmp_.ptr());
    }

private:
    mutable object tmp_;
};
}  // namespace cpy
