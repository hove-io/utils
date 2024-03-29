/* Copyright © 2001-2014, Hove and/or its affiliates. All rights reserved.

This file is part of Navitia,
    the software to build cool stuff with public transport.

Hope you'll enjoy and contribute to this project,
    powered by Hove (www.hove.com).
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

}  // namespace navitia
