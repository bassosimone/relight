#include <event2/bufferevent.h>
#include <functional>
#include <string>
#include "bytes.hpp"
#include "error-code.hpp"
#include "poller.hpp"
#include "net.hpp"
#include "utils-net.hpp"

namespace relight {

struct Socket {
    bufferevent *bufev = nullptr;
    Var<Poller> poller;
    Var<Socket> self;
    std::function<void()> delegate_connect;
    std::function<void(Var<Bytes>)> delegate_data;
    std::function<void()> delegate_flush;
    std::function<void(int)> delegate_error;
};

namespace net {

static void bufev_read(bufferevent *, void *ptr) {
    auto so = static_cast<relight::Socket *>(ptr);
    emit_libevent_read(so->self);
}

static void bufev_write(bufferevent *, void *ptr) {
    auto so = static_cast<relight::Socket *>(ptr);
    emit_libevent_write(so->self);
}

static void bufev_event(bufferevent *, short event, void *ptr) {
    auto so = static_cast<relight::Socket *>(ptr);
    emit_libevent_event(so->self, event);
}

Var<Socket> new_socket(Var<Poller> poller, evutil_socket_t filenum) {
    Var<Socket> s(new Socket);
    s->poller = poller;
    if ((s->bufev = bufferevent_socket_new(poller->get_event_base(), filenum,
                                           BEV_OPT_CLOSE_ON_FREE)) == nullptr) {
        throw std::bad_alloc();
    }
    s->self = s;
    bufferevent_setcb(s->bufev, bufev_read, bufev_write, bufev_event, s.get());
    if (filenum != -1 && bufferevent_enable(s->bufev, EV_READ) != 0) {
        throw std::runtime_error("bufferevent_enable");
    }
    return s;
}

void connect_ipv4(Var<Socket> socket, const char *addr, int port,
                  std::function<void()> cb, std::function<void(int)> eb) {
    ErrorCode code = relight_connect(socket->bufev, AF_INET, addr, port);
    if (code != 0) {
        eb(code);
        return;
    }
    on_connect(socket, cb);
    on_error(socket, eb);
}

void connect_ipv6(Var<Socket> socket, const char *addr, int port,
                  std::function<void()> cb, std::function<void(int)> eb) {
    ErrorCode code = relight_connect(socket->bufev, AF_INET6, addr, port);
    if (code != 0) {
        eb(code);
        return;
    }
    on_connect(socket, cb);
    on_error(socket, eb);
}

void write(Var<Socket> socket, Var<Bytes> bytes) {
    if (bufferevent_write_buffer(socket->bufev, bytes->get_evbuffer()) != 0) {
        throw std::runtime_error("evbuffer_write_buffer");
    }
}

void write(Var<Socket> socket, std::string s) {
    if (bufferevent_write(socket->bufev, s.c_str(), s.length()) != 0) {
        throw std::runtime_error("bufferevent_write");
    }
}

void on_connect(Var<Socket> socket, std::function<void()> f) {
    socket->delegate_connect = f;
}

void emit_connect(Var<Socket> socket) {
    if (socket->delegate_connect) {
        auto fn = socket->delegate_connect;
        fn();
    }
}

void on_data(Var<Socket> socket, std::function<void(Var<Bytes>)> f) {
    socket->delegate_data = f;
}

void emit_libevent_read(Var<Socket> socket) {
    evbuffer *source = bufferevent_get_input(socket->bufev);
    if (source == nullptr) {
        throw std::runtime_error("bufferevent_get_input");
    }
    Var<Bytes> bytes(new Bytes(source));
    emit_data(socket, bytes);
}

void emit_data(Var<Socket> socket, Var<Bytes> bytes) {
    if (socket->delegate_data) {
        auto fn = socket->delegate_data;
        fn(bytes);
    }
}

void on_flush(Var<Socket> socket, std::function<void()> f) {
    socket->delegate_flush = f;
}

void emit_libevent_write(Var<Socket> socket) { emit_flush(socket); }

void emit_flush(Var<Socket> socket) {
    if (socket->delegate_flush) {
        auto fn = socket->delegate_flush;
        fn();
    }
}

void on_error(Var<Socket> socket, std::function<void(int)> f) {
    socket->delegate_error = f;
}

void emit_libevent_event(Var<Socket> socket, short event) {
    if ((event & BEV_EVENT_CONNECTED)) {
        if (bufferevent_enable(socket->bufev, EV_READ) != 0) {
            throw std::runtime_error("bufferevent_enable");
        }
        emit_connect(socket);
    } else if ((event & BEV_EVENT_EOF)) {
        emit_error(socket, 3);
    } else if ((event & BEV_EVENT_TIMEOUT)) {
        emit_error(socket, 2);
    } else {
        emit_error(socket, 1);
    }
}

void emit_error(Var<Socket> socket, int error) {
    if (socket->delegate_error) {
        auto fn = socket->delegate_error;
        fn(error);
    } else {
        throw error;
    }
}

void close(Var<Socket> socket) {
    auto backup_dc = socket->delegate_connect;
    socket->delegate_connect = nullptr;
    auto backup_dd = socket->delegate_data;
    socket->delegate_data = nullptr;
    auto backup_df = socket->delegate_flush;
    socket->delegate_flush = nullptr;
    auto backup_de = socket->delegate_error;
    socket->delegate_error = nullptr;
    socket->self = nullptr;
    if (socket->bufev != nullptr) {
        bufferevent_free(socket->bufev);
        socket->bufev = nullptr;
    }
}

} // namespace net
} // namespace relight
