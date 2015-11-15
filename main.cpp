#include "bytes.hpp"
#include "poller.hpp"
#include "stream.hpp"
#include "var.hpp"

#ifdef SCRIPTED
#include "poller.cpp"
#include "stream.cpp"
#include "utils-net.cpp"
#endif

#include <iostream>

using namespace relight;

static void create_stream(Var<Poller> poller) {
    Var<Stream> stream(new Stream(poller));
    poller->run([=](std::function<void(std::function<void()>)> stop) {
        std::function<void()> cleanup = [=]() {
            poller->break_loop();
            stream->close();
            std::cerr << "break_with\n";
        };
        std::cerr << "with\n";
        stream->connect_ipv4("127.0.0.1", 8080, [=]() {
            std::cerr << "connected\n";
            stream->on_data([=](Var<Bytes>) {
                stream->on_flush([=]() {
                    stream->write("flushed\n");
                    stream->on_data([=](Var<Bytes>) {
                        stop(cleanup);
                    });
                    stream->on_flush([]() {});
                });
                stream->write("received first chunk\n");
            });
            stream->on_error([=](int err) {
                std::cerr << "error: " << err << "\n";
                stop(cleanup);
            });
        }, [=](int err) {
            std::cerr << "connect err: " << err << "\n";
            stop(cleanup);
        });
    });
}

int main() {
    Var<Poller> poller(new Poller);
    create_stream(poller);
    poller->loop();
}
