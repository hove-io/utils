/* Copyright © 2001-2018, Canal TP and/or its affiliates. All rights reserved.

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

#include <boost/optional.hpp>

namespace navitia {
namespace utils {

/**
 * @brief Find key within a map, in a functional way
 *
 * The find operation is performed when the object is constructed with a key,
 * or when find() is called.
 * Result is accessible via : if_found() or if_not_found()
 */
template <class Map>
class MapFind {
public:
    using KeyType = typename Map::key_type;
    using ValueType = typename Map::mapped_type;
    using ConstPtrValueType = const ValueType*;

    /**
     * @brief Create a MapFind object on a map, no find is performed until find() is called.
     *
     * @param map : the map to perform the search on
     */
    MapFind(Map& map) : map(map) {}

    /**
     * @brief Create a MapFind object and find the key in the associated map
     *
     * @param map : the map to perform the search on
     * @param key : the key to be searched in the map
     */
    MapFind(Map& map, const KeyType& key) : map(map) { find(key); }

    /**
     * @brief Callback fn with the value associated to the searched key
     * if the find was successful
     *
     * @param fn : a callback function that takes a constant mapped type reference as parameter
     * @returns A MapFind object to query the result or perform another find
     */
    template <class Func>
    MapFind& if_found(Func fn) {
        if (res) {
            fn(**res);
        }
        return *this;
    }

    /**
     * @brief Callback fn if nothing was found in the map
     *
     * @param fn : a callback function with no parameter
     * @returns A MapFind object to query the result or perform another find
     */
    template <class Func>
    MapFind& if_not_found(Func fn) {
        if (!res) {
            fn();
        }
        return *this;
    }

    /**
     * @brief Search for key within the map used in the constructor
     *
     * @param key : the key to be searched in the map
     * @returns A MapFind object to query the result or perform another find
     */
    template <class Key>
    MapFind& find(const Key& key) {
        auto it = map.find(key);
        if (it != map.end()) {
            res = boost::make_optional(&it->second);
        } else {
            res = boost::none;
        }
        return *this;
    }

private:
    Map& map;
    boost::optional<ConstPtrValueType> res;
};

/**
 * @brief Create a MapFind object with the associated map
 *
 * This function facilitates the construction of a MapFind
 * as it allows type deduction.
 * @param map : the map to perform the search on
 * @returns A MapFind object to query the result or perform another find
 */
template <class Map>
MapFind<Map> make_map_find(Map& map) {
    return MapFind<Map>(map);
}

/**
 * @brief Create a MapFind object with the associated map
 *
 * This function facilitates the construction of a MapFind
 * as it allows type deduction.
 * @param map : the map to perform the search on
 * @param key : the key to search in the map
 * @returns A MapFind object to query the result or perform another find
 */
template <class Map, class Key>
MapFind<Map> make_map_find(Map& c, const Key& key) {
    return MapFind<Map>(c, key);
}

}  // namespace utils
}  // namespace navitia
