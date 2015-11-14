#ifndef RELIGHT_UTILS_NET_HPP
#define RELIGHT_UTILS_NET_HPP

#include "error-code.hpp"

struct bufferevent;

ErrorCode relight_connect(bufferevent *bufev, int family,
                          const char *addr, int port);

#endif
