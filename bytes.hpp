#ifndef RELIGHT_BYTES_HPP
#define RELIGHT_BYTES_HPP

#include <event2/buffer.h>

#include <stdexcept>
#include <new>

#include <stddef.h>

struct evbuffer;

namespace relight {

class Bytes {
  public:
    Bytes() {
        if ((evbuf_ = evbuffer_new()) == nullptr) {
            throw std::bad_alloc();
        }
    }

    Bytes(Bytes &) = delete;
    Bytes &operator=(Bytes &) = delete;
    Bytes(Bytes &&) = delete;
    Bytes &operator=(Bytes &&) = delete;

    Bytes(const char *base, size_t count) : Bytes() {
        write(base, count);
    }

    explicit Bytes(evbuffer *buf) : Bytes() {
        write(buf);
    }

    evbuffer *get_evbuffer() { return evbuf_; }

    void write(evbuffer *source) {
        if (evbuffer_add_buffer(evbuf_, source) != 0) {
            throw std::runtime_error("evbuffer_add_buffer");
        }
    }

    void write(const char *base, size_t count) {
        if (evbuffer_add(evbuf_, base, count) != 0) {
            throw std::runtime_error("evbuffer_add");
        }
    }

    ~Bytes() {
        if (evbuf_ != nullptr) {
            evbuffer_free(evbuf_);
            evbuf_ = nullptr;
        }
    }

  private:
    evbuffer *evbuf_ = nullptr;
};

}
#endif
