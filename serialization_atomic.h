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
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_free.hpp>

#include <atomic>

namespace boost {
namespace serialization {

template <class Archive, class T>
inline void save(Archive& ar, const std::atomic<T>& t, const unsigned int) {
    // only the raw pointer has to be saved
    const T value = t.load();
    ar << value;
}

template <class Archive, class T>
inline void load(Archive& ar, std::atomic<T>& t, const unsigned int) {
    T value;
    ar >> value;
    t = value;
}

template <class Archive, class T>
inline void serialize(Archive& ar, std::atomic<T>& t, const unsigned int file_version) {
    boost::serialization::split_free(ar, t, file_version);
}
}  // namespace serialization
}  // namespace boost
