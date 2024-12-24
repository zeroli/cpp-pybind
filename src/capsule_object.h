#pragma once

#include "object.h"
#include "macro_define.h"

namespace cpy {
class capsule_t : public object {
public:
    using object::object;

    capsule_t(PyObject* obj, bool borrowed)
        : object(obj, borrowed)
    {}

    capsule_t(void* value, void (*destruct)(PyObject*) = nullptr)
        : object(PyCapsule_New(value, nullptr, destruct), false)
    {}

    template <typename T>
    operator T*() const {
        T* result = static_cast<T*>(PyCapsule_GetPointer(ptr_, nullptr));
        CHECK(result);
        return result;
    }
};
}  // namespace cpy
