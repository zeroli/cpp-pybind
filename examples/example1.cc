#include "python2.7/Python.h"

#include <iostream>

#include "module.h"
#include "class.h"

namespace {
class A {
public:
    A() { std::cout << "A:ctor\n"; }
    ~A() { std::cout << "A:dtor\n"; }

    void foo1() const {
        std::cout << "A::foo1\n";
    }

    static void soo() {
        std::cout << "static A::soo\n";
    }
private:
    int x{1};
};
}  // namespace

static void registerA(cpy::module& m)
{
    cpy::class_<A>(m, "A")
        .def("foo1", &A::foo1)
        .def_static("soo", &A::soo);
}

int main(int argc, char** argv)
{
    cpy::module m;
    registerA(m);
}
