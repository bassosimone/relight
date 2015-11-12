#ifndef RELIGHT_STREAM_HPP
#define RELIGHT_STREAM_HPP

#include "bytes.hpp"
#include "poller.hpp"
#include "var.hpp"

#include <event2/bufferevent.h>
#include <event2/event.h>

#include <functional>
#include <memory>
#include <string>

#include <stddef.h>

extern "C" {
void relight_bufev_event(bufferevent *, short, void *);
void relight_bufev_read(bufferevent *, void *);
void relight_bufev_write(bufferevent *, void *);
}

#ifdef RELIGHT_NAMESPACE
namespace RELIGHT_NAMESPACE {
#endif

class Stream {
  public:
    Stream(Var<Poller> poller, evutil_socket_t filenum) : poller_(poller) {
        if ((bufev_ = bufferevent_socket_new(poller->get_event_base(), filenum,
                                             BEV_OPT_CLOSE_ON_FREE)) == nullptr)
            throw std::bad_alloc();
        bufferevent_setcb(bufev_, relight_bufev_read, relight_bufev_write,
                          relight_bufev_event, this);
        if (bufferevent_enable(bufev_, EV_READ) != 0) {
            throw std::runtime_error("bufferevent_enable");
        }
    }

    Stream(Stream &) = delete;
    Stream &operator=(Stream &) = delete;
    Stream(Stream &&) = delete;
    Stream &operator=(Stream &&) = delete;

    void write(Var<Bytes> bytes) {
        if (bufferevent_write_buffer(bufev_, bytes->get_evbuffer()) != 0) {
            throw std::runtime_error("evbuffer_write_buffer");
        }
    }

    void write(std::string s) {
        if (bufferevent_write(bufev_, s.c_str(), s.length()) != 0) {
            throw std::runtime_error("bufferevent_write");
        }
    }

    void on_data(std::function<void(Var<Bytes>)> f) { data_fn_ = f; }

    void emit_read() {
        auto evbuff = bufferevent_get_input(bufev_);
        if (evbuff == nullptr) {
            throw std::runtime_error("bufferevent_get_input");
        }
        Var<Bytes> bytes(new Bytes(evbuff));
        emit_data(bytes);
    }

    void emit_data(Var<Bytes> bytes) {
        if (data_fn_) {
            auto fn = data_fn_;
            fn(bytes);
        }
    }

    void on_flush(std::function<void()> f) { flush_fn_ = f; }

    void emit_flush() {
        if (flush_fn_) {
            auto fn = flush_fn_;
            fn();
        }
    }

    void on_error(std::function<void(int)> f) { error_fn_ = f; }

    void emit_event(short event) {
        if ((event & BEV_EVENT_CONNECTED)) {
            emit_error(1);
        } else if ((event & BEV_EVENT_EOF)) {
            emit_error(3);
        } else if ((event & BEV_EVENT_TIMEOUT)) {
            emit_error(2);
        } else {
            emit_error(1);
        }
    }

    void emit_error(int error) {
        if (error_fn_) {
            auto fn = error_fn_;
            fn(error);
        }
    }

    void close() {
        if (bufev_ != nullptr) {
            bufferevent_free(bufev_);
            data_fn_ = nullptr;
            flush_fn_ = nullptr;
            error_fn_ = nullptr;
            bufev_ = nullptr;
        }
    }

    ~Stream() { close(); }

  private:
    bufferevent *bufev_ = nullptr;
    Var<Poller> poller_;
    std::function<void(Var<Bytes>)> data_fn_;
    std::function<void()> flush_fn_;
    std::function<void(int)> error_fn_;
};

#ifdef RELIGHT_NAMESPACE
}
#endif

#endif
