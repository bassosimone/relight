#include "utils-net.h"

#include <event2/bufferevent.h>
#include <event2/util.h>

#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>

ErrorCode relight_connect(struct bufferevent *bufev, int family,
                          const char *addr, int port) {
    if (bufferevent_getfd(bufev) != -1) {
        return 10;
    }
    if (family == AF_INET) {
        struct sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        if (evutil_inet_pton(AF_INET, addr, &sin.sin_addr) != 1) {
            return 20;
        }
    } else if (family == AF_INET6) {
        struct sockaddr_in6 sin6;
        memset(&sin6, 0, sizeof(sin6));
        sin6.sin6_family = AF_INET6;
        if (evutil_inet_pton(AF_INET6, addr, &sin6.sin6_addr) != 1) {
            return 20;
        }
    } else {
        return 30;
    }
    if (bufferevent_socket_connect_hostname(bufev, NULL, family, addr,
                                            port) != 0) {
        return 40;
    }
    return 0;
}
