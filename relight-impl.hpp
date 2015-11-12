#ifndef RELIGHT_IMPL_HPP
#define RELIGHT_IMPL_HPP

#ifndef RELIGHT_NAMESPACE
#define RELIGHT_NAMESPACE
#endif

#include "stream.hpp"

void relight_once_cb(evutil_socket_t, short, void *ptr) {
    auto funcptr = static_cast<std::function<void()> *>(ptr);
    (*funcptr)();
    delete funcptr;
}

void relight_bufev_read(bufferevent *, void *ptr) {
    auto so = static_cast<RELIGHT_NAMESPACE::Stream *>(ptr);
    so->emit_read();
}

void relight_bufev_write(bufferevent *, void *ptr) {
    auto so = static_cast<RELIGHT_NAMESPACE::Stream *>(ptr);
    so->emit_flush();
}

void relight_bufev_event(bufferevent *, short event, void *ptr) {
    auto so = static_cast<RELIGHT_NAMESPACE::Stream *>(ptr);
    so->emit_event(event);
}

#endif
