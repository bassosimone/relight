#ifndef RELIGHT_DNS_HPP
#define RELIGHT_DNS_HPP

#include <functional>
#include <list>
#include <string>

namespace relight {

typedef std::function<void(int, std::list<std::string>)> dns_callback;

void dns_resolve4(std::string domain, dns_callback);

}
#endif
