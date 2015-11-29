#include "dns.hpp"
#include "dns-resolver.hpp"

#include <limits.h>

#include <arpa/inet.h>

typedef std::function<void(int, char, int, int, void *)> evdns_callback;

// TODO: C callbacks should be declared with C linkage
static void handle_resolve(int code, char type, int count, int ttl,
                           void *addresses, void *opaque) {
    evdns_callback *callback = static_cast<evdns_callback *>(opaque);
    (*callback)(code, type, count, ttl, addresses);
    delete callback;
}

// The following is derived from more general code in measurement-kit:
static std::vector<std::string> ipv4_address_list(int count, void *addresses) {
    std::vector<std::string> results;
    static const int size = 4;
    char string[128]; // Is wide enough (max. IPv6 length is 45 chars)
    if (count >= 0 && count <= INT_MAX / size + 1) {
        for (int i = 0; i < count; ++i) {
            // Note: address already in network byte order
            if (inet_ntop(AF_INET, (char *)addresses + i * size, string,
                          sizeof(string)) == nullptr) {
                break;
            }
            results.push_back(string);
        }
    }
    return results;
}

// The following is copied from measurement-kit
static std::vector<std::string> reverse_address_list(void *addresses) {
    std::vector<std::string> results;
    results.push_back(std::string(*(char **)addresses));
    return results;
}

namespace relight {
namespace dns {

void resolve4(std::string domain, callback callback) {
    Var<Resolver> resolver = Resolver::get_default();
    resolver->get_poller()->call_soon([=]() {
        // double ticks = 0.0; // TODO
        evdns_callback *funcptr = new evdns_callback(
            [=](int code, char, int count, int, void *addresses) {
                callback(code, ipv4_address_list(count, addresses));
            });
        std::vector<std::string> empty;
        if (evdns_base_resolve_ipv4(resolver->get_evdns_base(), domain.c_str(),
                                    DNS_QUERY_NO_SEARCH, handle_resolve,
                                    funcptr) == nullptr) {
            callback(DNS_ERR_UNKNOWN, empty);
            delete funcptr;
            return;
        }
    });
}

void reverse4(std::string address, callback callback) {
    Var<Resolver> resolver = Resolver::get_default();
    resolver->get_poller()->call_soon([=]() {
        std::vector<std::string> empty;
        in_addr netaddr;
        if (inet_pton(AF_INET, address.c_str(), &netaddr) != 1) {
            callback(DNS_ERR_UNKNOWN, empty);
            return;
        }
        evdns_callback *funcptr = new evdns_callback(
            [=](int code, char, int, int, void *addresses) {
                callback(code, reverse_address_list(addresses));
            });
        if (evdns_base_resolve_reverse(resolver->get_evdns_base(), &netaddr,
                                       DNS_QUERY_NO_SEARCH, handle_resolve,
                                       funcptr) == nullptr) {
            callback(DNS_ERR_UNKNOWN, empty);
            delete funcptr;
            return;
        }
    });
}

} // namespace dns
} // namespace relight
