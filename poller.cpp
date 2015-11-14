#ifndef RELIGHT_NAMESPACE
#define RELIGHT_NAMESPACE
#endif

#include <functional>

#include <event2/util.h>

#include "poller.hpp"

void relight_once_cb(evutil_socket_t, short, void *ptr) {
    auto funcptr = static_cast<std::function<void()> *>(ptr);
    (*funcptr)();
    delete funcptr;
}
