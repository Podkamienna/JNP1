#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include <functional>


inline auto compose() {
    return [](auto x) { return x; };
}

template<typename F, typename ...Args>
auto compose(F f, Args... args) {
    return [=](auto ...x) { return compose(args...)(f(x...)); };
}

template<typename F, typename ...Args>
auto lift(F f, Args...args) {
    return [=](auto x) {
        return std::invoke(f, args(x)...);
    };
}


#endif //FUNCTIONAL_H
