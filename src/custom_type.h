#pragma once

#include "object.h"
#include <typeinfo>

namespace cpy {
class custom_type : public object
{
public:
    using object::object;

    custom_type(object& scope, const char* name,
        const std::type_info* tinfo,
        size_t type_size,
        PyObject* parent = nullptr)
    {
        // TODO
    }
private:
};
}  // namespace cpy
