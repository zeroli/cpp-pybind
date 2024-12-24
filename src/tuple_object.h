#pragma once

#include "object.h"
#include "macro_define.h"

namespace cpy {
namespace detail {
class tuple_accessor
{
public:
    tuple_accessor(PyObject* tuple, size_t index)
        : tuple_(tuple), index_(index)
    { }

    /// assign one handle to tuple item reference
    void operator =(const handle& o) {
        o.inc_ref();
        if (PyTuple_SetItem(tuple_, (ssize_t)index_, (PyObject*)o.ptr()) < 0) {
            CHECK(0);
        }
    }

    /// return the underlying object
    operator object() const {
        PyObject* result = PyTuple_GetItem(tuple_, index_);
        CHECK(result);

        return object(result, true);
    }
private:
    PyObject* tuple_;
    size_t index_;
};
}  // namespace detail

class tuple_t : public object {
public:
    using object::object;

    tuple_t(size_t size)
        : object(PyTuple_New((Py_ssize_t)size), false)
    {}

    size_t size() const {
        return (size_t)PyTuple_Size(ptr_);
    }

    detail::tuple_accessor operator [](size_t index) {
        return detail::tuple_accessor(ptr(), index);
    }
};
}  // namespace cpy
