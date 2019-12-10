#include "get_hostname.h"
#include <unistd.h>

namespace navitia {
std::string get_hostname() {
    char hostname[1024];
    ::gethostname(hostname, 1024);
    return std::string(hostname);
}
}  // namespace navitia
