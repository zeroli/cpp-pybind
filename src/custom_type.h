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
                        size_t instance_size,
                        void (*init_holder)(PyObject *),
                        const destructor& dealloc,
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
        type->ht_type.tp_basicsize = instance_size;
        type->ht_type.tp_init = (initproc)init;
        type->ht_type.tp_new = (newfunc)new_instance;
        type->ht_type.tp_dealloc = dealloc;
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
        type_info.init_holder = init_holder;

        attr(CPP_PYBIND) = capsule_t(&type_info);
        scope.attr(name_obj) = *this;
    }
protected:
    static int init(void* self, PyObject*, PyObject*)
    {
        std::string msg = std::string(Py_TYPE(self)->tp_name) + ": No constructor defined!";
        PyErr_SetString(PyExc_TypeError, msg.c_str());
        return -1;
    }

    static PyObject* new_instance(PyTypeObject* type, PyObject*, PyObject*)
    {
        const detail::type_info* type_info = capsule_t(
            PyObject_GetAttrString((PyObject*)type, const_cast<char*>(CPP_PYBIND)),
            false
        );
        detail::instance<void>* self = (detail::instance<void>*)PyType_GenericAlloc(type, 0);
        self->value = ::operator new(type_info->type_size);
        self->owned = true;
        self->parent = nullptr;
        self->constructed = false;
        detail::get_internals().registered_instances_[self->value] = (PyObject*)self;
        return (PyObject*)self;
    }

    static void dealloc(detail::instance<void>* self)
    {
        if (self->value) {
            bool dont_cache = self->parent
                && ((detail::instance<void> *) self->parent)->value == self->value;
            if (!dont_cache) {
                auto& registered_instances = detail::get_internals().registered_instances_;
                auto it = registered_instances.find(self->value);
                CHECK(it != registered_instances.end());
                registered_instances.erase(it);
            }
            Py_XDECREF(self->parent);
        }
        Py_TYPE(self)->tp_free((PyObject*)self);
    }
};
}  // namespace cpy
