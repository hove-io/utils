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
#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_free.hpp>
#include <memory>

namespace boost {
namespace serialization {

template <class Archive, class T>
inline void save(Archive& ar, const std::unique_ptr<T>& t, const unsigned int) {
    // only the raw pointer has to be saved
    const T* const tx = t.get();
    ar << tx;
}

template <class Archive, class T>
inline void load(Archive& ar, std::unique_ptr<T>& t, const unsigned int) {
    T* pTarget;
    ar >> pTarget;

#if BOOST_WORKAROUND(BOOST_DINKUMWARE_STDLIB, == 1)
    t.release();
    t = std::unique_ptr<T>(pTarget);
#else
    t.reset(pTarget);
#endif
}

template <class Archive, class T>
inline void serialize(Archive& ar, std::unique_ptr<T>& t, const unsigned int file_version) {
    boost::serialization::split_free(ar, t, file_version);
}

}  // namespace serialization
}  // namespace boost
