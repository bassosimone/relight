#include "relight-impl.hpp"

int main() {
    Var<Poller> poller(new Poller);
    Var<Stream> stream(new Stream(poller, 0));
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
    poller->loop();
    stream->close();
}
