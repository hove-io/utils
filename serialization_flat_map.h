/* Copyright © 2001-2014, Canal TP and/or its affiliates. All rights reserved.

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

// from boost/serialization/map.hpp with the following copyright:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/container/flat_map.hpp>

#include <boost/serialization/map.hpp>

namespace boost { namespace serialization {

template<class Archive, class ...Types>
inline void save(
    Archive & ar,
    const boost::container::flat_map<Types...> &t,
    const unsigned int /* file_version */
){
    boost::serialization::stl::save_collection<Archive, boost::container::flat_map<Types...>>(ar, t);
}

template<class Archive, class ...Types>
inline void load(
    Archive & ar,
    boost::container::flat_map<Types...> &t,
    const unsigned int /* file_version */
){
    load_map_collection(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class ...Types>
inline void serialize(
    Archive & ar,
    boost::container::flat_map<Types...> &t,
    const unsigned int file_version
){
    boost::serialization::split_free(ar, t, file_version);
}

}} // namespace boost::serialization
