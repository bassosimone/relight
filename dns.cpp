#include "dns.hpp"
#include "resolver.hpp"

#include <limits.h>

#include <arpa/inet.h>

typedef std::function<void(int, char, int, int, void *)> evdns_callback;

static void handle_resolve(int code, char type, int count, int ttl,
                           void *addresses, void *opaque) {
    evdns_callback *callback = static_cast<evdns_callback *>(opaque);
    (*callback)(code, type, count, ttl, addresses);
    delete callback;
}

static std::list<std::string> ipv4_address_list(int count, void *addresses) {
    std::list<std::string> results;
    static const int size = 4;
    char string[128]; // Is wide enough (max. IPv6 length is 45 chars)
    if (count >= 0 && count <= INT_MAX / size + 1) {
        for (int i = 0; i < count; ++i) {
            // Note: address already in network byte order
            if (inet_ntop(AF_INET, (char *)addresses + i * size,
                          string, sizeof(string)) == nullptr) {
                break;
            }
            results.push_back(string);
        }
    }
    return results;
}

namespace relight {

void dns_resolve4(std::string domain, dns_callback callback) {
    Var<Resolver> resolver = Resolver::get_default();
    resolver->get_poller()->call_soon([=]() {
        //double ticks = 0.0; // TODO
        evdns_callback *funcptr = new evdns_callback(
            [=](int code, char, int count, int, void *addresses) {
                callback(code, ipv4_address_list(count, addresses));
            });
        std::list<std::string> empty;
        if (evdns_base_resolve_ipv4(resolver->get_evdns_base(), domain.c_str(),
                                    DNS_QUERY_NO_SEARCH, handle_resolve,
                                    funcptr) == nullptr) {
            callback(DNS_ERR_UNKNOWN, empty);
            delete funcptr;
            return;
        }
    });

}

} // namespace
