#include "bytes.hpp"
#include "dns.hpp"
#include "error-code.hpp"
#include "poller.hpp"
#include "resolver.hpp"
#include "stream.hpp"
#include "utils-net.hpp"
#include "var.hpp"

#ifdef SCRIPTED
#include "dns.cpp"
#include "poller.cpp"
#include "stream.cpp"
#include "utils-net.cpp"
#endif

#include <iostream>

using namespace relight;

static void make_request() {
    dns_resolve4("www.torproject.org", [](int err, std::list<std::string> addrs) {
        if (err) {
            std::cerr << "Error: " << err << "\n";
            return;
        }
        for (auto &s : addrs) std::cout << "    - " << s << "\n";
        Poller::get_default()->break_loop();
    });
}

int main() {
    make_request();
    Poller::get_default()->loop();
}
