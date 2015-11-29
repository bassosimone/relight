#ifndef RELIGHT_FOR_EACH_HPP
#define RELIGHT_FOR_EACH_HPP

#include "poller.hpp"
#include "var.hpp"

#include <functional>
#include <vector>

namespace relight {

typedef std::function<void()> next_func;

template <typename T>
void for_each(std::vector<T> vec,
              std::function<void(T, next_func)> on_elem,
              std::function<void()> on_end,
              Var<Poller> poller = nullptr,
              size_t pos = 0) {
    if (!poller) poller = Poller::get_default();
    poller->call_soon([=]() {
        try {
            on_elem(vec.at(pos), [=]() {
                for_each<T>(vec, on_elem, on_end, poller, pos + 1);
            });
        } catch (std::out_of_range &) {
            on_end();
            return;
        }
    });
}

}
#endif
