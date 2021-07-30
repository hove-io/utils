/* Copyright © 2001-2015, Canal TP and/or its affiliates. All rights reserved.

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
#include <boost/range/iterator_range_core.hpp>

#include <vector>
#include <iterator>

namespace navitia {

using idx_t = uint32_t;
const idx_t invalid_idx = std::numeric_limits<idx_t>::max();

// Strong typing of index with a phantom type!
template <typename T>
struct Idx {
    inline explicit Idx() : val(invalid_idx) {}
    inline explicit Idx(const idx_t& v) : val(v) {}
    inline explicit Idx(const T& o) : val(o.idx) {}
    inline bool is_valid() const { return val != invalid_idx; }
    inline bool operator==(const Idx& other) const { return val == other.val; }
    inline bool operator!=(const Idx& other) const { return val != other.val; }
    inline bool operator<(const Idx& other) const { return val < other.val; }
    inline friend std::ostream& operator<<(std::ostream& os, const Idx& idx) { return os << idx.val; }

    idx_t val;  // the value of the index
};

template <typename K, typename I>
class IdxMapIterator : public boost::iterator_facade<IdxMapIterator<K, I>,
                                                     std::pair<const K, typename std::iterator_traits<I>::reference>,
                                                     boost::random_access_traversal_tag,
                                                     std::pair<const K, typename std::iterator_traits<I>::reference> > {
public:
    using traits = typename std::iterator_traits<I>;
    using difference_type = typename traits::difference_type;

    inline IdxMapIterator() = default;
    inline IdxMapIterator(const idx_t& i, I it) : idx(i), iterator(it) {}

private:
    friend class boost::iterator_core_access;

    inline void increment() {
        ++idx;
        ++iterator;
    }
    inline void decrement() {
        --idx;
        --iterator;
    }
    inline void advance(difference_type n) {
        idx += n;
        iterator += n;
    }
    inline difference_type distance_to(const IdxMapIterator& other) const { return other.iterator - iterator; }
    inline bool equal(const IdxMapIterator& other) const { return iterator == other.iterator; }
    inline std::pair<const K, typename traits::reference> dereference() const { return {K(idx), *iterator}; }

    idx_t idx;
    I iterator;
};

// A HashMap with optimal hash!
template <typename T, typename V>
struct IdxMap {
    using key_type = Idx<T>;
    using mapped_type = V;
    using container = std::vector<V>;
    using iterator = IdxMapIterator<key_type, typename container::iterator>;
    using const_iterator = IdxMapIterator<key_type, typename container::const_iterator>;
    using range = boost::iterator_range<typename std::vector<V>::iterator>;
    using const_range = boost::iterator_range<typename std::vector<V>::const_iterator>;

    inline IdxMap() = default;
    inline IdxMap(const std::vector<T*>& c, const V& val = V()) : map(c.size(), val) {}
    inline IdxMap(const std::vector<T>& c, const V& val = V()) : map(c.size(), val) {}
    inline IdxMap(const size_t size, const V& val = V()) : map(size, val) {}

    inline friend void swap(IdxMap& lhs, IdxMap& rhs) {
        using std::swap;
        swap(lhs.map, rhs.map);
    }

    // initialize the map with the number of element
    // we give the container for type checking
    inline void assign(const std::vector<T*>& c, const V& val = V()) { map.assign(c.size(), val); }
    inline void assign(const std::vector<T>& c, const V& val = V()) { map.assign(c.size(), val); }
    template <typename U>
    inline void assign(const IdxMap<T, U>& c, const V& val = V()) {
        map.assign(c.size(), val);
    }

    // resize the map
    inline void resize(const size_t size) { map.resize(size); }
    inline void resize(const std::vector<T*>& c) { map.resize(c.size()); }
    inline void resize(const std::vector<T>& c) { map.resize(c.size()); }
    template <typename U>
    inline void resize(const IdxMap<T, U>& c) {
        map.resize(c.size());
    }

    // accessors
    inline size_t size() const { return map.size(); }
    inline const V& operator[](const Idx<T>& idx) const { return map[idx.val]; }
    inline V& operator[](const Idx<T>& idx) { return map[idx.val]; }
    inline V& at(const Idx<T>& idx) { return map.at(idx.val); }
    inline const V& at(const Idx<T>& idx) const { return map.at(idx.val); }

    // iterator getters
    inline iterator begin() { return iterator(0, map.begin()); }
    inline iterator end() { return iterator(map.size(), map.end()); }
    inline const_iterator begin() const { return const_iterator(0, map.cbegin()); }
    inline const_iterator end() const { return const_iterator(map.size(), map.cend()); }
    inline const_iterator cbegin() const { return const_iterator(0, map.cbegin()); }
    inline const_iterator cend() const { return const_iterator(map.size(), map.cend()); }

    // iterate on const values
    inline const_range values() const { return boost::make_iterator_range(map.cbegin(), map.cend()); }

    // iterate on values
    inline range values() { return boost::make_iterator_range(map.begin(), map.end()); }

private:
    container map;
};

}  // namespace navitia
