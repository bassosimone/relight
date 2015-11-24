#ifndef RELIGHT_NET_HPP
#define RELIGHT_NET_HPP

#include <functional>
#include <string>
#include "var.hpp"

namespace relight {

class Bytes;
class Poller;
struct Socket;

namespace net {

Var<Socket> new_socket(Var<Poller> poller);

void connect_ipv4(Var<Socket> socket, const char *addr, int port,
                  std::function<void()> cb, std::function<void(int)> eb);

void connect_ipv6(Var<Socket> socket, const char *addr, int port,
                  std::function<void()> cb, std::function<void(int)> eb);

void write(Var<Socket> socket, Var<Bytes> bytes);

void write(Var<Socket> socket, std::string s);

void on_connect(Var<Socket> socket, std::function<void()> f);

void emit_connect(Var<Socket> socket);

void on_data(Var<Socket> socket, std::function<void(Var<Bytes>)> f);

void emit_libevent_read(Var<Socket> socket);

void emit_data(Var<Socket> socket, Var<Bytes> bytes);

void on_flush(Var<Socket> socket, std::function<void()> f);

void emit_libevent_write(Var<Socket> socket);

void emit_flush(Var<Socket> socket);

void on_error(Var<Socket> socket, std::function<void(int)> f);

void emit_libevent_event(Var<Socket> socket, short);

void emit_error(Var<Socket> socket, int error);

void close(Var<Socket> socket);

} // namespace net
} // namespace relight
#endif
