#pragma once

#include "custom_type.h"
#include "cpp_function.h"

#include <memory>

namespace cpy {
namespace detail {
template <typename... Args>
struct init;
}  // namespace detail

template <typename T, typename holder_t = std::unique_ptr<T>>
class class_ : public custom_type
{
public:
    using instance_type = detail::instance<T, holder_t>;

    using custom_type::custom_type;

    class_(object& scope, const char* name)
        : custom_type(
            scope, name, &typeid(T), sizeof(T), sizeof(instance_type),
            class_::init_holder, class_::dealloc
        )
    {}

    class_(object& scope, const char* name, object& parent)
        : custom_type(
            scope, name, &typeid(T), sizeof(T), sizeof(instance_type),
            class_::init_holder, class_::dealloc, parent.ptr()
        )
    { }

    /// define constructor
    template <typename... Args, typename... Extra>
    class_& def(const detail::init<Args...>& init, Extra&&... extra)
    {
        init.template execute<T>(*this, std::forward<Extra>(extra)...);
        return *this;
    }

    /// define class method
    template <typename Func, typename... Extra>
    class_& def(const char* name, Func&& func, Extra&&... extra)
    {
        cpp_function cf(std::forward<Func>(func),
                                name,
                                true,  /// is_method
                                std::forward<Extra>(extra)...);
        attr(cf.name()) = cf;
        return *this;
    }

    /// define class static function
    template <typename Func, typename... Extra>
    class_& def_static(const char* name, Func&& func, Extra&&... extra)
    {
        cpp_function cf(std::forward<Func>(func),
                                name,
                                false, /// is not method
                                std::forward<Extra>(extra)...);
        attr(cf.name()) = cf;
        return *this;
    }
private:
    static void init_holder(PyObject* instance)
    {
        instance_type* inst = (instance_type*)instance;
        new (&inst->holder) holder_t(inst->value);
        inst->constructed = true;
    }
    static void dealloc(PyObject* instance)
    {
        instance_type* inst = (instance_type*)instance;
        if (inst->owned) {
            if (inst->constructed) {
                inst->holder.~holder_t();
            } else {
                ::operator delete(inst->value);
            }
        }
        custom_type::dealloc((detail::instance<void>*)inst);
    }
};

namespace detail {
template <typename... Args>
struct init
{
    template <typename T, typename Holder, typename... Extra>
    void execute(class_<T, Holder>& tclass, Extra&&... extra) const
    {
        tclass.def("__init__", [](T* instance, Args... args) {
                new (instance)T(std::forward<Args>(args)...);
            },
            std::forward<Extra>(extra)...
        );
    }
};
}  // namespace detail

/// utility function to make a "init"
template <typename... Args>
detail::init<Args...> init()
{
    return detail::init<Args...>();
}

}  // namespace cpy
