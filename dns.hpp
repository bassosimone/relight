#ifndef RELIGHT_DNS_HPP
#define RELIGHT_DNS_HPP

#include <functional>
#include <string>
#include <vector>

namespace relight {
namespace dns {

typedef std::function<void(int, std::vector<std::string>)> callback;

void resolve4(std::string domain, callback);

void reverse4(std::string name, callback);

}
}
#endif
