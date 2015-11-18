#ifndef RELIGHT_DNS_HPP
#define RELIGHT_DNS_HPP

#include <functional>
#include <list>
#include <string>

namespace relight {
namespace dns {

typedef std::function<void(int, std::list<std::string>)> callback;

void resolve4(std::string domain, callback);

}
}
#endif
