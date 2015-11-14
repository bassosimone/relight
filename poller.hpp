#ifndef RELIGHT_POLLER_HPP
#define RELIGHT_POLLER_HPP

#include "var.hpp"

#include <event2/event.h>
#include <event2/util.h>

#include <functional>
#include <map>
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

    template <typename T> void with(Var<T> t, std::function<void(
            std::function<void(std::function<void()>)>)> init) {
        T *p = t.get();
        if (running_.find(p) != running_.end()) {
            throw std::runtime_error("element already there");
        }
        running_[p] = [=]() {};  // keep t safe
        std::function<void(std::function<void()>)> finish = [=](
                std::function<void()> cleanup) {
            running_.erase(p);
            call_soon([=]() { cleanup(); });
        };
        call_soon([=]() { init(finish); });
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
    std::map<void *, std::function<void()>> running_;
};

}
#endif
