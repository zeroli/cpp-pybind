#pragma once

#include "object.h"
#include "macro_define.h"
#include "internals.h"

#include <typeinfo>

namespace cpy {
class custom_type : public object
{
public:
    using object::object;

    explicit custom_type(object& scope, const char* name,
                        const std::type_info* tinfo,
                        size_t type_size,
                        PyObject* parent = nullptr)
    {
        PyHeapTypeObject* type = (PyHeapTypeObject*)PyType_Type.tp_alloc(&PyType_Type, 0);
        PyObject* name_obj = PyString_FromString(name);
        CHECK(type != nullptr && name_obj != nullptr);

        Py_INCREF(name_obj);
        std::string full_name(name);
        str_t scope_name = (object)scope.attr("__name__");
        str_t module_name = (object)scope.attr("__module__");
        if (scope_name.ok()) {
            full_name = std::string(scope_name) + "." + full_name;
        }
        if (module_name.ok()) {
            full_name = std::string(module_name) + "." + full_name;
        }

        type->ht_name = name_obj;
        type->ht_type.tp_name = strdup(full_name.c_str());
        type->ht_type.tp_basicsize = 0; // instance_size;
        type->ht_type.tp_init = (initproc)nullptr; //init;
        type->ht_type.tp_new = (newfunc)nullptr; //new_instance;
        type->ht_type.tp_dealloc = nullptr; //dealloc;
        type->ht_type.tp_flags |=
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HEAPTYPE;
        type->ht_type.tp_flags &= ~Py_TPFLAGS_HAVE_GC;
        type->ht_type.tp_flags |= Py_TPFLAGS_CHECKTYPES;
        type->ht_type.tp_as_number = &type->as_number;
        type->ht_type.tp_as_sequence = &type->as_sequence;
        type->ht_type.tp_as_mapping = &type->as_mapping;
        type->ht_type.tp_base = (PyTypeObject *) parent;
        Py_XINCREF(parent);
        CHECK(PyType_Ready(&type->ht_type) >= 0);

        ptr_ = (PyObject *) type;
        attr("__module__") = scope_name;

        auto &type_info = detail::get_internals().registered_types_[tinfo];
        type_info.type = (PyTypeObject *) ptr_;
        type_info.type_size = type_size;
        type_info.init_holder = nullptr; //init_holder;

        attr("__cpp-pybind__") = capsule_t(&type_info);
        scope.attr(name_obj) = *this;
    }
private:
};
}  // namespace cpy
