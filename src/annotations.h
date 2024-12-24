#pragma once

#include "object.h"

namespace cpy {
template <typename T>
struct arg_t;

/// annotation for keyword arguments
struct arg {
    arg(const char* name) : name_(name) {}

    template <typename T>
    inline arg_t<T> operator =(const T& value);

    const char* name_;
};

/// annotation for keyword arguments with default value
template <typename T>
struct arg_t : public arg {
    arg_t(const char* name, const T& value)
        : arg(name), value_(value)
    {}

    T value;
};

template <typename T>
inline arg_t<T> arg::operator =(const T& value)
{
    return arg_t<T>(name_, value);
}

/// annotation for methods
struct is_method {
    PyObject* class_;
    is_method(object* o)
        : class_(o->ptr())
    {}
};

/// annotation for function name
struct name {
    const char* value_;

    name(const char* value)
        : value_(value)
    { }
};

/// annotation for function siblings
struct sibling {
    PyObject* value_;

    sibling(handle val)
        : value_(val.ptr())
    {}
};
}  // namespace cpy
