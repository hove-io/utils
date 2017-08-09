#pragma once
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <utility>

class wrong_coordinate: public std::runtime_error{using runtime_error::runtime_error;};

// the new way to represent a coord is : "lon;lat"
const std::string match_double = "[-+]?[0-9]*\\.?[0-9]*[eE]?[-+]?[0-9]*";
const auto coord_regex = boost::regex("^(" + match_double + ");(" + match_double + ")$");

/**
 *
 * return the coord as pair<lon, lat>
 */
std::pair<double, double> parse_coordinate(const std::string& uri){
    if (uri.size() > 6 && uri.substr(0, 6) == "coord:") {
        // old style to represent coord.
        // done for retrocompatibility
        size_t pos2 = uri.find(":", 6);
        if (pos2 != std::string::npos) {
            double lon = boost::lexical_cast<double>(uri.substr(6, pos2 - 6));
            double lat = boost::lexical_cast<double>(uri.substr(pos2+1));
            return std::make_pair(lon, lat);
        }
    }else{
        boost::smatch matches;
        bool res = boost::regex_match(uri, matches, coord_regex);
        if (res && matches.size() == 3) {
            double lon = boost::lexical_cast<double>(matches[1]);
            double lat = boost::lexical_cast<double>(matches[2]);
            return std::make_pair(lon, lat);
        }
    }
    throw wrong_coordinate{"coord format invalid"};
}
