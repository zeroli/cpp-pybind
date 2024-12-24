#pragma once

#include "object.h"

#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <memory>

namespace cpy {
namespace detail {

/// Information record describing a Python buffer object
struct buffer_info {
    void *ptr;
    size_t itemsize, count;
    std::string format; // for dense contents, this should be set to format_descriptor<T>::value
    int ndim;
    std::vector<size_t> shape;
    std::vector<size_t> strides;

    buffer_info(void *ptr, size_t itemsize, const std::string &format, int ndim,
                const std::vector<size_t> &shape, const std::vector<size_t> &strides)
        : ptr(ptr), itemsize(itemsize), format(format)
        , ndim(ndim), shape(shape), strides(strides)
    {
        count = 1;
        for (int i=0; i<ndim; ++i) count *= shape[i];
    }
};

/// PyObject wrapper around generic types
template <typename type, typename holder_type = std::unique_ptr<type>>
struct instance {
    PyObject_HEAD
    type *value;
    PyObject *parent;
    bool owned : 1;
    bool constructed : 1;
    holder_type holder;
};

/// Additional type information which does not fit into the PyTypeObjet
struct type_info {
    PyTypeObject *type;
    size_t type_size;
    void (*init_holder)(PyObject *);
    std::vector<PyObject *(*)(PyObject *, PyTypeObject *)> implicit_conversions;
    buffer_info *(*get_buffer)(PyObject *, void *) = nullptr;
    void *get_buffer_data = nullptr;
};

struct overload_hash {
    inline std::size_t operator()(const std::pair<const PyObject *, const char *>& v) const
    {
        size_t value = std::hash<const void *>()(v.first);
        value ^= std::hash<const void *>()(v.second)  + 0x9e3779b9 + (value<<6) + (value>>2);
        return value;
    }
};

/// Internal data struture used to track registered instances and types
struct internals {
    std::unordered_map<const std::type_info *, type_info> registered_types_;
    std::unordered_map<const void *, PyObject *> registered_instances_;
    std::unordered_set<std::pair<const PyObject *, const char *>, overload_hash> inactive_overload_cache_;
};

inline internals& get_internals()
{
    static internals* internals_ptr = nullptr;
    if (internals_ptr) {
        return *internals_ptr;
    }

    handle builtins(PyEval_GetBuiltins());
    capsule_t caps(builtins[CPP_PYBIND]);
    if (caps.ok()) {
        internals_ptr = caps;
    } else {
        internals_ptr = new internals();
        builtins[CPP_PYBIND] = capsule_t(internals_ptr);
    }
    return *internals_ptr;
}
}  // namespace detail
}  // namespace cpy
