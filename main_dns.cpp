#include "bytes.hpp"
#include "dns.hpp"
#include "dns-resolver.hpp"
#include "error-code.hpp"
#include "for-each.hpp"
#include "net.hpp"
#include "poller.hpp"
#include "stream.hpp"
#include "var.hpp"

#ifdef SCRIPTED
#include "dns.cpp"
#include "net.cpp"
#include "poller.cpp"
#endif

#include <iostream>

using namespace relight;

static void make_request() {
    dns::resolve4("www.torproject.org",
            [](int err, std::vector<std::string> addrs) {
        if (err) throw err;
        for_each<std::string>(
            Poller::get_default(), addrs,
            [=](std::string s, next_func next) {
                std::cout << "    - " << s;
                dns::reverse4(s, [=](int err, std::vector<std::string> revs) {
                    if (err) throw err;
                    for (auto &s : revs) std::cout << " " << s;
                    std::cout << "\n";
                    next();
                });
            },
            [=]() { Poller::get_default()->break_loop(); });
    });
}

int main() {
    make_request();
    Poller::get_default()->loop();
}
