#pragma once
#include <string>

namespace navitia {
    std::string base64_encode(const std::string& input);

/**
  * for uris we might want to encode them or not, so it can be
  * activated with the ENABLE_URI_ENCODING variable at compile time
  **/

#ifdef ENABLE_URI_ENCODING
    inline std::string encode_uri(const std::string& input) {
        return base64_encode(input);
    }
#else
    inline const std::string& encode_uri(const std::string& input) {
        return input;
    }
#endif

}
