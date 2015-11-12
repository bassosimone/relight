#ifndef RELIGHT_POLLER_HPP
#define RELIGHT_POLLER_HPP

#include <event2/event.h>

#include <functional>
#include <memory>
#include <stdexcept>

#include <stddef.h>

#ifdef RELIGHT_NAMESPACE
namespace RELIGHT_NAMESPACE {
#endif

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

#ifdef RELIGHT_NAMESPACE
}
#endif
#endif
