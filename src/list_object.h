#pragma once

#include "object.h"
#include "macro_define.h"

namespace cpy {
namespace detail {
class list_iterator {
public:
    list_iterator(PyObject* list, size_t pos)
        : list_(list), pos_(pos)
    {}

    list_iterator& operator++() {
        ++pos_;
        return *this;
    }

    object operator *() {
        return object(PyList_GetItem(list_, (Py_ssize_t)pos_), true);
    }

    bool operator ==(const list_iterator& it) const {
        return pos_ == it.pos_;
    }

    bool operator !=(const list_iterator& it) const {
        return !(*this == it);
    }
private:
    PyObject* list_;
    size_t pos_;
};

class list_accessor {
public:
    list_accessor(PyObject* list, size_t index)
        : list_(list), index_(index)
    {}

    // assign an object to list item reference
    void operator =(const handle& o) {
        o.inc_ref();
        if (PyList_SetItem(list_, (ssize_t)index_, (PyObject*)o.ptr()) < 0) {
            CHECK(0);
        }
    }

    // dereference the list item
    operator object() const {
        auto* result = PyList_GetItem(list_, (ssize_t)index_);
        CHECK(result);
        return object(result, true);
    }

private:
    PyObject* list_;
    size_t index_;
};
}  // namespace detail

class list_t : public object {
public:
    using object::object;

    list_t(size_t size = 0)
        : object(PyList_New((ssize_t)size), false)
    {}

    size_t size() const {
        return (size_t)PyList_Size(ptr_);
    }

    // iteration
    detail::list_iterator begin() {
        return detail::list_iterator(ptr(), 0);
    }
    detail::list_iterator end() {
        return detail::list_iterator(ptr(), size());
    }

    detail::list_accessor operator [](size_t index) {
        return detail::list_accessor(ptr(), index);
    }

    void append(const object& object) {
        PyList_Append(ptr(), (PyObject*)object.ptr());
    }
};
}  // namespace cpy
