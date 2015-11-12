#include "relight-impl.hpp"
#include <iostream>

int main() {
    Var<Poller> poller(new Poller);
    Var<Stream> stream(new Stream(poller));
    stream->connect_ipv4("127.0.0.1", 8080, [=]() {
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
    }, [=](int err) {
        std::cout << "error " << err << "\n";
        poller->break_loop();
    });
    poller->loop();
    stream->close();
}
