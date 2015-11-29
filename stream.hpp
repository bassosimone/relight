#ifndef RELIGHT_STREAM_HPP
#define RELIGHT_STREAM_HPP

#include <functional>
#include <string>
#include "net.hpp"
#include "var.hpp"

namespace relight {

class Bytes;
class Poller;

class Stream {
    // Implementation note: this is just a frontend to provide object
    // semantic to an otherwise C style API. In turn, the good thing about
    // the C style API is that objects are only kept alive by means of
    // smart pointers, thus leading to safer code. And this proxy doesn't
    // change the game either, since the only piece of data that it does
    // contain is indeed just the smart pointer thus increasing refs.
  public:
    explicit Stream(Var<Poller> poller) : socket(net::new_socket(poller)) {}

    Stream(Var<Poller> poller, evutil_socket_t filenum)
            : socket(net::new_socket(poller, filenum)) {}

    void connect_ipv4(const char *addr, int port, std::function<void()> cb,
                      std::function<void(int)> eb) const {
        net::connect_ipv4(socket, addr, port, cb, eb);
    }

    void connect_ipv6(const char *addr, int port, std::function<void()> cb,
                      std::function<void(int)> eb) const {
        net::connect_ipv6(socket, addr, port, cb, eb);
    }

    void write(Var<Bytes> bytes) const {
        net::write(socket, bytes);
    }

    void write(std::string s) const {
        net::write(socket, s);
    }

    void on_connect(std::function<void()> f) const {
        net::on_connect(socket, f);
    }

    void emit_connect() const { net::emit_connect(socket); }

    void on_data(std::function<void(Var<Bytes>)> f) const {
        net::on_data(socket, f);
    }

    void emit_read() const { net::emit_libevent_read(socket); }

    void emit_data(Var<Bytes> bytes) const { net::emit_data(socket, bytes); }

    void on_flush(std::function<void()> f) const { net::on_flush(socket, f); }

    void emit_flush() const { net::emit_flush(socket); }

    void on_error(std::function<void(int)> f) const {
        net::on_error(socket, f);
    }

    void emit_event(short event) const {
        net::emit_libevent_event(socket, event);
    }

    void emit_error(int error) const { net::emit_error(socket, error); }

    void close() const { net::close(socket); }

    ~Stream() {}

  private:
    Var<Socket> socket;
};

}
#endif
