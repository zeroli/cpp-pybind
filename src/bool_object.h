#pragma once

#include "object.h"

namespace cpy {
class bool_t : public object {
public:
    using object::object;

    operator bool() const {
        return ptr_ && PyLong_AsLong(ptr_) != 0;
    }
};
}  // namespace cpy
