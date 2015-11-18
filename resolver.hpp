#ifndef RELIGHT_RESOLVER_HPP
#define RELIGHT_RESOLVER_HPP

#include "poller.hpp"
#include "var.hpp"

#include <event2/dns.h>

namespace relight {
namespace dns {

class Resolver {
  public:
    Resolver() {}

    Resolver(Var<Poller> poller) : poller_(poller) {}

    Resolver(Resolver &) = delete;
    Resolver &operator=(Resolver &) = delete;
    Resolver(Resolver &&) = delete;
    Resolver &operator=(Resolver &&) = delete;

    evdns_base *get_evdns_base() {
        if (base_ == nullptr) {
            base_ = evdns_base_new(poller_->get_event_base(), 1);
            if (base_ == nullptr) throw std::bad_alloc();
        }
        return base_;
    }

    Var<Poller> get_poller() { return poller_; }

    ~Resolver() {
        if (base_ != nullptr) {
            evdns_base_free(base_, 0);
            base_ = nullptr;
        }
    }

    static Var<Resolver> get_default() {
        static Var<Resolver> singleton;
        if (!singleton) singleton.reset(new Resolver);
        return singleton;
    }

  private:
    Var<Poller> poller_ = Poller::get_default();
    evdns_base *base_ = nullptr;
};

}
}
#endif
