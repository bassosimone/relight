#include "bytes.hpp"
#include "dns.hpp"
#include "dns-resolver.hpp"
#include "error-code.hpp"
#include "for-each.hpp"
#include "net.hpp"
#include "poller.hpp"
#include "net.hpp"
#include "var.hpp"

#ifdef SCRIPTED
#include "dns.cpp"
#include "poller.cpp"
#include "net.cpp"
#endif

#include <iostream>

using namespace relight;

static void create_socket(Var<Poller> poller) {
    Var<Socket> socket = net::new_socket(poller);
    net::connect_ipv4(socket, "127.0.0.1", 8080, [=]() {
        std::cerr << "connected\n";
        net::on_data(socket, [=](Var<Bytes>) {
            net::on_flush(socket, [=]() {
                net::write(socket, "flushed\n");
                net::on_data(socket, [=](Var<Bytes>) {
                    net::close(socket);
                    poller->break_loop();
                });
                net::on_flush(socket, nullptr);
            });
            net::write(socket, "received first chunk\n");
        });
        net::on_error(socket, [=](int err) {
            std::cerr << "error: " << err << "\n";
            net::close(socket);
            poller->break_loop();
        });
    }, [=](int err) {
        std::cerr << "connect err: " << err << "\n";
        net::close(socket);
        poller->break_loop();
    });
}

int main() {
    Var<Poller> poller(new Poller);
    create_socket(poller);
    poller->loop();
}
