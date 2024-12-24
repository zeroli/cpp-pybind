#pragma once

#include "wrap_python.h"

namespace cpy {

class str_t;

namespace detail {
class accessor;
}  // namespace detail

// a simple wrapper to PyObject without reference counting
class handle {
public:
    handle() = default;

    handle(PyObject* ptr) : ptr_(ptr) { }
    handle(const handle& other) = default;
    handle& operator =(const handle& other) = default;
    handle(handle&& other) noexcept = default;
    handle& operator =(handle&& other) = default;

    PyObject* ptr() { return ptr_; }
    const PyObject* ptr() const { return ptr_; }

    void inc_ref() const {
        Py_XINCREF(ptr_);
    }
    void dec_ref() const {
        Py_XDECREF(ptr_);
    }

    int ref_count() const {
        return (int)Py_REFCNT(ptr_);
    }

    handle get_type() const {
        return handle((PyObject*)Py_TYPE(ptr_));
    }

    inline detail::accessor operator[](handle key);
    inline detail::accessor operator[](const char* key);
    inline detail::accessor attr(handle key);
    inline detail::accessor attr(const char* key);

    inline str_t str() const;

    // template <typename T>
    // T cast();

    // template <typename... Args>
    // object call(Args&&... args);

    operator bool() const {
        return ptr_ != nullptr;
    }
    bool ok() const {
        return ptr_ != nullptr;
    }
protected:
    PyObject* ptr_ {nullptr};
};

/// holds a reference to a python object with reference counting
class object : public handle
{
public:
    object() = default;
    object(const object& rhs)
        : handle(rhs)
    {
        inc_ref();
    }
    object(const handle& rhs, bool borrowed = true)
        : handle(rhs)
    {
        if (borrowed) {
            inc_ref();
        }
    }
    object(PyObject* ptr, bool borrowed = true)
        : handle(ptr)
    {
        if (borrowed) {
            inc_ref();
        }
    }
    object(object&& rhs) {
        ptr_ = rhs.ptr_;
        rhs.ptr_ = nullptr;
    }
    ~object() {
        dec_ref();
    }

    object& operator =(const object& other) {
        other.inc_ref();
        dec_ref();
        ptr_ = other.ptr_;
        return *this;
    }

    object& operator =(object&& other) {
        if (this != &other) {
            dec_ref();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }
};

namespace detail {
class accessor {
public:
    accessor(PyObject* obj, PyObject* key, bool is_attr)
        : obj_(obj), key_(key), is_attr_(is_attr)
    {
        Py_INCREF(key);
    }
    accessor(PyObject* obj, const char* key, bool is_attr)
        : obj_(obj), key_(PyUnicode_FromString(key)), is_attr_(is_attr)
    {
    }
    accessor(const accessor& other)
        : obj_(other.obj_), key_(other.key_), is_attr_(other.is_attr_)
    {
        Py_INCREF(key_);
    }
    ~accessor() {
        Py_DECREF(key_);
    }

    void operator =(const handle& h) {
        if (is_attr_) {
            if (PyObject_SetAttr(obj_, key_, (PyObject*)h.ptr()) < 0)
            {
                assert(0 && "Unable to set object attr");
            }
        } else {
            if (PyObject_SetItem(obj_, key_, (PyObject*)h.ptr()) < 0)
            {
                assert(0 && "Unable to set object item");
            }
        }
    }

    operator object() const {
        object result(is_attr_
                ? PyObject_GetAttr(obj_, key_)
                : PyObject_GetItem(obj_, key_),
                false);
        if (!result) PyErr_Clear();
        return result;
    }

    operator bool() const {
        if (is_attr_) {
            return (bool)PyObject_HasAttr(obj_, key_);
        } else {
            object result(PyObject_GetItem(obj_, key_), false);
            if (!result) PyErr_Clear();
            return (bool)result;
        }
    }
private:
    PyObject* obj_;
    PyObject* key_;
    bool is_attr_;
};
}  // namespace detail

inline detail::accessor handle::operator[](handle key)
{
    return detail::accessor(ptr(), key.ptr(), false);
}

inline detail::accessor handle::operator[](const char* key)
{
    return detail::accessor(ptr(), key, false);
}

inline detail::accessor handle::attr(handle key)
{
    return detail::accessor(ptr(), key.ptr(), true);
}

inline detail::accessor handle::attr(const char* key)
{
    return detail::accessor(ptr(), key, true);
}

}  // namespace cpy

#include "bool_object.h"
#include "capsule_object.h"
#include "dict_object.h"
#include "float_object.h"
#include "int_object.h"
#include "list_object.h"
#include "slice_object.h"
#include "str_object.h"
#include "tuple_object.h"

namespace cpy {
inline str_t handle::str() const
{
    PyObject* str = PyObject_Str(ptr_);
    PyObject* unicode = PyUnicode_FromEncodedObject(str, "utf-8", nullptr);
    Py_XDECREF(str);
    str = unicode;
    return str_t(str, false);
}
}  // namespace cpy
