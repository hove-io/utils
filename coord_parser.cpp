/* Copyright © 2001-2017, Canal TP and/or its affiliates. All rights reserved.

This file is part of Navitia,
    the software to build cool stuff with public transport.

Hope you'll enjoy and contribute to this project,
    powered by Canal TP (www.canaltp.fr).
Help us simplify mobility and open public transport:
    a non ending quest to the responsive locomotion way of traveling!

LICENCE: This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

Stay tuned using
twitter @navitia
IRC #navitia on freenode
https://groups.google.com/d/forum/navitia
www.navitia.io
*/

#include "coord_parser.h"

#include <boost/lexical_cast.hpp>

#include <utility>

namespace navitia {

wrong_coordinate::~wrong_coordinate() noexcept = default;

static std::pair<double, double> make_coord(const std::string& lon, const std::string& lat) {
    try {
        auto dlon = boost::lexical_cast<double>(lon);
        auto dlat = boost::lexical_cast<double>(lat);
        return {dlon, dlat};
    } catch (const boost::bad_lexical_cast&) {
        throw wrong_coordinate("conversion failed");
    }
}

static const std::string match_double = "[-+]?[0-9]*\\.?[0-9]*[eE]?[-+]?[0-9]*";
const boost::regex coord_regex = boost::regex("^(" + match_double + ");(" + match_double + ")$");

/**
 *
 * return the coord as pair<lon, lat>
 */
std::pair<double, double> parse_coordinate(const std::string& uri) {
    // the new way to represent a coord is : "lon;lat"

    if (uri.size() > 6 && uri.substr(0, 6) == "coord:") {
        // old style to represent coord.
        // done for retrocompatibility
        size_t pos2 = uri.find(':', 6);
        if (pos2 != std::string::npos) {
            return make_coord(uri.substr(6, pos2 - 6), uri.substr(pos2 + 1));
        }
    } else {
        boost::smatch matches;
        bool res = boost::regex_match(uri, matches, coord_regex);
        if (res && matches.size() == 3) {
            return make_coord(matches[1], matches[2]);
        }
    }
    throw wrong_coordinate("coord format invalid");
}

}  // namespace navitia
