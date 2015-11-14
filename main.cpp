#include "bytes.hpp"
#include "poller.hpp"
#include "stream.hpp"
#include "var.hpp"

#include <iostream>

using namespace relight;

int main() {
    Var<Poller> poller(new Poller);
    Var<Stream> stream(new Stream(poller));
    poller->call_soon([=]() {
        std::cerr << "loop running\n";
        stream->connect_ipv4("127.0.0.1", 8080, [=]() {
            std::cerr << "connected\n";
            stream->on_data([=](Var<Bytes>) {
                stream->on_flush([=]() {
                    stream->write("flushed\n");
                    stream->on_data([=](Var<Bytes>) {
                        poller->break_loop();
                    });
                    stream->on_flush([]() {});
                });
                stream->write("received first chunk\n");
            });
            stream->on_error([=](int err) {
                std::cerr << "error: " << err << "\n";
                poller->break_loop();
            });
        }, [=](int err) {
            std::cerr << "connect err: " << err << "\n";
            poller->break_loop();
        });
    });
    poller->loop();
    std::cerr << "disconnected\n";
    stream->close();
}
