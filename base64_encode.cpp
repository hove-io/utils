#include "base64_encode.h"
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <sstream>
#include <string>

// copy paste from http://stackoverflow.com/questions/7053538/how-do-i-encode-a-string-to-base64-using-only-boost
namespace navitia {

std::string base64_encode(const std::string& input) {
    namespace bi = boost::archive::iterators;
    std::stringstream os;
    using base64_text = bi::base64_from_binary<bi::transform_width<const char *, 6, 8>>;
    std::copy(
            base64_text(input.c_str()),
            base64_text(input.c_str() + input.size()),
            bi::ostream_iterator<char>(os)
            ); 
    return os.str();
}
}
