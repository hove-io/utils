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
#include <boost/serialization/split_member.hpp>
#include <memory>
#include <map>

#include <boost/serialization/wrapper.hpp>
#include "serialization_unique_ptr.h"

/**
 * Serialization specialization for std::vector<std::unique_ptr> because unique_ptr are not copiable
 */
namespace boost {
namespace serialization {

template <class Archive, class Allocator, class T>
inline void save(Archive& ar,
                 const std::vector<std::unique_ptr<T>, Allocator>& t,
                 const unsigned int /* file_version */) {
    // record number of elements
    collection_size_type count(t.size());
    ar << BOOST_SERIALIZATION_NVP(count);

    for (const auto& elt : t)
        ar << boost::serialization::make_nvp("item", elt);
}

template <class Archive, class Allocator, class T>
inline void load(Archive& ar, std::vector<std::unique_ptr<T>, Allocator>& t, const unsigned int /* file_version */) {
    // retrieve number of elements
    collection_size_type count;
    ar >> BOOST_SERIALIZATION_NVP(count);
    t.clear();
    while (count-- > 0) {
        std::unique_ptr<T> i;
        ar >> boost::serialization::make_nvp("item", i);
        t.push_back(std::move(i));  // move object
    }
}

template <class Archive, class k, class V, class Comp, class MapAlloc>
inline void save(Archive& ar,
                 const std::map<k, std::unique_ptr<V>, Comp, MapAlloc>& t,
                 const unsigned int /* file_version */) {
    // record number of elements
    collection_size_type count(t.size());
    ar << BOOST_SERIALIZATION_NVP(count);

    for (const auto& p : t) {
        ar << boost::serialization::make_nvp("key", p.first);
        ar << boost::serialization::make_nvp("value", p.second);
    }
}

template <class Archive, class K, class V, class Comp, class MapAlloc>
inline void load(Archive& ar,
                 std::map<K, std::unique_ptr<V>, Comp, MapAlloc>& map,
                 const unsigned int /* file_version */) {
    // retrieve number of elements
    collection_size_type count;
    ar >> BOOST_SERIALIZATION_NVP(count);
    map.clear();
    while (count-- > 0) {
        K k;
        ar >> boost::serialization::make_nvp("key", k);
        std::unique_ptr<V> v;
        ar >> boost::serialization::make_nvp("value", v);
        map[k] = std::move(v);  // move object
    }
}

template <class Archive, class Allocator, class T>
inline void serialize(Archive& ar, std::vector<std::unique_ptr<T>, Allocator>& t, const unsigned int file_version) {
    boost::serialization::split_free(ar, t, file_version);
}

template <class Archive, class K, class V, class Comp, class Allocator>
inline void serialize(Archive& ar,
                      std::map<K, std::unique_ptr<V>, Comp, Allocator>& m,
                      const unsigned int file_version) {
    boost::serialization::split_free(ar, m, file_version);
}
}  // namespace serialization
}  // namespace boost
