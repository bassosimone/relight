#ifndef RELIGHT_UTILS_NET_H
#define RELIGHT_UTILS_NET_H

#include "error-code.h"

struct bufferevent;

#ifdef __cplusplus
extern "C" {
#endif

ErrorCode relight_connect(struct bufferevent *bufev, int family,
                          const char *addr, int port);

#ifdef __cplusplus
}
#endif

#endif
