// Well, I actually really hate this.

#include "bytes.hpp"
#include "dns.hpp"
#include "error-code.hpp"
#include "for-each.hpp"
#include "net.hpp"
#include "poller.hpp"
#include "resolver.hpp"
#include "stream.hpp"
#include "var.hpp"

#ifdef SCRIPTED
#include "dns.cpp"
#include "net.cpp"
#include "poller.cpp"
#endif

#include <iostream>

using namespace relight;

static void create_stream(Var<Poller> poller) {
    Stream stream(poller);
    std::function<void()> cleanup = [=]() {
        poller->break_loop();
        stream.close();
        std::cerr << "break_with\n";
    };
    stream.connect_ipv4("127.0.0.1", 8080, [=]() {
        // Object lifetime note: stream is copyable / movable etc. and
        // so is cleanup, thus everything is safe and alive.
        std::cerr << "connected\n";
        stream.on_data([=](Var<Bytes>) {
            stream.on_flush([=]() {
                stream.write("flushed\n");
                stream.on_data([=](Var<Bytes>) {
                    cleanup();
                });
                stream.on_flush([]() {});
            });
            stream.write("received first chunk\n");
        });
        stream.on_error([=](int err) {
            std::cerr << "error: " << err << "\n";
            cleanup();
        });
    }, [=](int err) {
        std::cerr << "connect err: " << err << "\n";
        cleanup();
    });
}

int main() {
    Var<Poller> poller(new Poller);
    create_stream(poller);
    poller->loop();
}
