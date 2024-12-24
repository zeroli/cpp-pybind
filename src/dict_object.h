#pragma once

#include "object.h"
#include "macro_define.h"

namespace cpy {
namespace detail {
class dict_iterator {
public:
    dict_iterator() = default;
    dict_iterator(PyObject* dict, ssize_t pos)
        : dict_(dict), pos_(pos)
    {}

    dict_iterator& operator ++() {
        if (!PyDict_Next(dict_, &pos_, &key_, &value_)) {
            pos_ = -1;
        }
        return *this;
    }

    std::pair<object, object> operator *() const {
        return std::make_pair(object(key_, true), object(value_, true));
    }

    bool operator ==(const dict_iterator& it) const {
        return pos_ == it.pos_;
    }
    bool operator !=(const dict_iterator& it) const {
        return !(*this == it);
    }
private:
    PyObject* dict_ {nullptr};
    PyObject* key_ {nullptr};
    PyObject* value_ {nullptr};

    ssize_t pos_ {-1};
};
}  // namespace detail

class dict_t : public object {
public:
    using object::object;

    dict_t()
        : object(PyDict_New(), false)
    {}

    size_t size() const {
        return (size_t)PyDict_Size(ptr_);
    }

    /// iteration
    detail::dict_iterator begin() {
        return (++detail::dict_iterator(ptr(), 0));
    }
    detail::dict_iterator end() {
        return detail::dict_iterator();
    }
};
}  // namespace cpy
