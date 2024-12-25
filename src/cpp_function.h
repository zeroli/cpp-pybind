#pragma once

#include "function.h"
#include "type_traits.h"

namespace cpy {
enum return_value_policy {
    automatic = 0,
    take_ownership,
    reference,
    reference_internal,
    copy,
};
class cpp_function : public function
{
public:
    // chained list os function entries for overloading
    struct function_entry {
        const char* name { nullptr };
        PyObject* (*impl)(function_entry*, PyObject*, PyObject*, PyObject*) { nullptr} ;
        PyMethodDef* def { nullptr };
        void* data { nullptr };
        bool is_constructor { false };
        bool is_method { false };
        short keywords { 0 };
        void (*free)(void* ptr) { nullptr };
        return_value_policy policy = return_value_policy::automatic;
        std::string signature;
        PyObject* class_ { nullptr };
        PyObject* sibling { nullptr };

        function_entry* next { nullptr };
    };

    using function::function;

    // return and arguments caster
    template <typename... T>
    using arg_value_caster =
        detail::type_caster<typename std::tuple<T...>>;

    template <typename... T>
    using return_value_caster =
        detail::type_caster<
            std::conditional_t<
                std::is_void_t<T>,
                    detail::void_t,
                    std::decay_t<T>
            >
        >;

    template <typename R, typename... Args, typename... Extra>
    cpp_function(R (*f)(Args...), Extra&&... extra)
    {
        // TODO
    }

    // delegating helper ctor to deal with lambda function
    template <typename Func, typename... Extra>
    cpp_function(Func&& func, Extra&&... extra)
    {
        initialize(std::forward<Func>(func),
                    (detail::remove_class_t<
                        decltype(&std::remove_reference_t<Func>::operator())
                     >*)nullptr,
                    std::forward<Extra>(extra)...
        );
    }

    // non-const method
    template <typename R, typename C, typename... Args, typename... Extra>
    cpp_function(R (C::*func)(Args...), Extra&&... extra)
    {
        initialize([func](C* c, Args... args) -> R {
                    return (c->*func)(std::forward<Args>(args)...);
                },
                (R (*)(C*, Args...))nullptr,
                std::forward<Extra>(extra)...
        );
    }

    // const method
    template <typename R, typename C, typename... Args, typename... Extra>
    cpp_function(R (C::*func)(Args...) const, Extra&&... args)
    {
        initialize([func](C* c, Args... args) -> R {
                    return (c->*func)(std::forward<Args>(args)...);
                },
                (R (*)(C*, Args...))nullptr,
                std::forward<Extra>(extra)...
        );
    }

    const char *name() const { return entry_->name; }

private:
    function_entry* entry_ { nullptr };
private:
    /// functors, lambda functions, etc
    /// the 2nd argument is telling R, A for that function signature: F
    template <typename F, typename R, typename... A, typename... Extra>
    void initialize(F&& func, R (*)(A...), Extra&&... extra)
    {
        struct capture {
            std::remove_reference_t<Func> f;
            std::tuple<Extra...> extras;
        };

        entry_ = new function_entry();
        entry->data = new capture{
            std::forward<Func>(func),
            std::tuple<Extra...>(std::forward<Extra>(extra)...)
        };

        if (!std::is_trivially_destructible_v<Func>) {
            entry->free = [] (void* ptr) { delete (capture*)ptr; };
        }

        using cast_in = arg_value_caster<A...>;
        using cast_out = return_value_caster<R>;

        entry_->impl = [](function_entry* entry, PyObject* pyargs, PyObject* kwargs, PyObject* parent) -> PyObject*
        {
            capture* data = (capture*)entry->data;
            //process_extras(data->extras, pyargs, kwargs,
            cast_in args;
            if (!args.load(pyargs, true)) {
                return (PyObject*)1;  // special return code, try next overload
            }
            return cast_out::cast(args.template call<R>(data->f), entry->policy, parent);
        }
    }
};
}  // namespace cpy
