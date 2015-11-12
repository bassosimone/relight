#include "relight-impl.hpp"

int main() {
    Var<Poller> poller(new Poller);
    Var<Stream> socket(new Stream(poller, 0));
    socket->on_data([=](Var<Bytes>) {
        socket->on_flush([=]() {
            socket->write("flushed\n");
            socket->on_data([=](Var<Bytes>) {
                socket->close();
                poller->break_loop();
            });
            socket->on_flush([]() {});
        });
        socket->write("received first chunk\n");
    });
    poller->loop();
}
