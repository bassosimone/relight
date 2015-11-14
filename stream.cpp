#ifndef RELIGHT_NAMESPACE
#define RELIGHT_NAMESPACE
#endif

#include "stream.hpp"

struct bufferevent;

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
