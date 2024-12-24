#pragma once

#include "object.h"

#include "int_object.h"

namespace cpy {
class slice_t : public object {
public:
    using object::object;

    slice_t(ssize_t start, ssize_t stop, ssize_t step) {
        int_t istart(start), istop(stop), istep(step);
        ptr_ = PySlice_New(istart.ptr(), istop.ptr(), istep.ptr());
    }

};
}  // namespace cpy
