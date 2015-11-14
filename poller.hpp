#ifndef RELIGHT_POLLER_HPP
#define RELIGHT_POLLER_HPP

#include "var.hpp"

#include <event2/event.h>
#include <event2/util.h>

#include <functional>
#include <new>
#include <stdexcept>

struct event_base;

// Libevent callbacks should be C style functions
extern "C" {
void relight_once_cb(evutil_socket_t, short, void *);
}

namespace relight {

class Poller {
  public:
    Poller() {
        if ((evbase_ = event_base_new()) == nullptr) {
            throw std::bad_alloc();
        }
    }

    Poller(Poller &) = delete;
    Poller& operator=(Poller &) = delete;
    Poller(Poller &&) = delete;
    Poller& operator=(Poller &&) = delete;

    event_base *get_event_base() { return evbase_; }

    void call_soon(std::function<void()> func) {
        auto funcptr = new std::function<void()>(func);
        if (event_base_once(evbase_, -1, EV_TIMEOUT, relight_once_cb,
                            funcptr, nullptr) != 0) {
            throw std::runtime_error("event_base_once");
        }
    }

    void run(std::function<void(std::function<
            void(std::function<void()>)>)> init) {
        call_soon([=]() {
            init([=](std::function<void()> cleanup) {
                call_soon([=]() { cleanup(); });
            });
        });
    }

    void loop() {
        if (event_base_dispatch(evbase_) != 0) {
            throw std::runtime_error("event_base_dispatch");
        }
    }

    void break_loop() {
        if (event_base_loopbreak(evbase_) != 0)
            throw std::runtime_error("event_base_loopbreak");
    }

    ~Poller() {
        if (evbase_ != nullptr) {
            event_base_free(evbase_);
            evbase_ = nullptr;
        }
    }

  private:
    event_base *evbase_ = nullptr;
};

}
#endif
