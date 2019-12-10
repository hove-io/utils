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
#include <array>
#include <type_traits>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range_core.hpp>

namespace navitia {

// template <typename EnumKey> struct enum_size_trait {};

// for the moment the compiler version does not support std::underlying type
#ifdef GCC_NOT_TOO_OLD
template <typename Enum>
struct get_enum_type {
    typedef std::underlying_type<Enum>::type type;
};
#else
template <typename Enum>
struct get_enum_type {
    typedef int type;
};
#endif

/**
 * Default specialization, use 'size' field that has to be the last
 * Specialize it for the enum that does not have a 'size' element at the end
 */
template <typename Enum>
struct enum_size_trait {
    static constexpr typename get_enum_type<Enum>::type size() {
        return static_cast<typename get_enum_type<Enum>::type>(Enum::size);
    }
};

/**
 * Helper, the first elt of the enum should always be 0
 */
template <typename Enum>
Enum get_first_elt() {
    return static_cast<Enum>(0);
}

template <typename EnumKey, typename Value>
class flat_enum_map_iterator;
/**
 * Simple container associating an enum value to a value.
 *
 * The mapped enum MUST have its first value initialized to 0
 *
 * to get the number of elements in the enum we can either define a 'size' last element
 * or specialize the enum_size_trait for the enum
 *
 * the underlying type is a std::array for performance concern
 *
 * Note:
 * Due to a gcc bug http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57086
 * the flat_enum_container has to be an aggregate (else it won't work nor compile if stored in vector)
 * so no constructor can be defined thus the array cannot be default initialized
 */
template <typename EnumKey, typename Value>
struct flat_enum_map {
    typedef std::array<Value, enum_size_trait<EnumKey>::size()> underlying_container;
    typedef flat_enum_map_iterator<EnumKey, Value> iterator;
    typedef flat_enum_map_iterator<EnumKey, const Value> const_iterator;

    underlying_container array;

    //    flat_enum_map() = default;
    //    flat_enum_map(flat_enum_map& v) : array(v.array){}
    //    flat_enum_map(const flat_enum_map& v) = default;
    //    flat_enum_map(flat_enum_map&&) = default;
    //    flat_enum_map& operator=(const flat_enum_map&) = default;

    //    template<typename ...Val>
    //    flat_enum_map(Val&& ...v) : array{{std::forward<Val>(v)...}} {}

    Value& operator[](EnumKey key) { return array[size_t(key)]; }

    constexpr const Value& operator[](EnumKey key) const { return array[size_t(key)]; }

    const Value& at(EnumKey key) const {
        auto idx = size_t(key);
        if (idx >= array.size()) {
            throw std::out_of_range("enum not in range");
        }
        return array[idx];
    }

    constexpr const Value& operator[](size_t idx) const { return array[idx]; }

    template <class Archive>
    void serialize(Archive& ar, const unsigned int) {
        ar& boost::serialization::make_array(array.data(),
                                             array.size());  // default serialization not available in boost 1.48
    }

    const_iterator begin() const { return const_iterator(array.begin(), get_first_elt<EnumKey>()); }
    const_iterator end() const { return const_iterator(array.end()); }
    iterator begin() { return iterator(array.begin(), get_first_elt<EnumKey>()); }
    iterator end() { return iterator(array.end()); }
};

template <typename Enum>
class enum_iterator
    : public boost::iterator_facade<enum_iterator<Enum>, Enum, boost::random_access_traversal_tag, Enum> {
    typedef typename get_enum_type<Enum>::type underlying_type;
    underlying_type _it;

public:
    typedef std::make_signed<typename get_enum_type<Enum>::type> difference_type;
    enum_iterator() : _it(enum_size_trait<Enum>::size()) {}
    enum_iterator(const Enum& e) : _it(static_cast<underlying_type>(e)) {}
    enum_iterator(const underlying_type& i) : _it(i) {}

    void increment() { ++_it; }
    void decrement() { --_it; }
    void advance(difference_type n) { _it += n; }

    difference_type distance_to(const enum_iterator& other) { return _it - other._it; }
    bool equal(const enum_iterator& other) const { return _it == other._it; }
    Enum dereference() const { return static_cast<Enum>(_it); }
};

template <typename Enum>
boost::iterator_range<enum_iterator<Enum>> enum_range() {
    typedef enum_iterator<Enum> it;
    return boost::make_iterator_range(it(get_first_elt<Enum>()), it());
}

template <typename Enum>
boost::iterator_range<boost::reverse_iterator<enum_iterator<Enum>>> reverse_enum_range() {
    typedef enum_iterator<Enum> it;
    typedef boost::reverse_iterator<it> rit;
    return boost::make_iterator_range(rit(it(enum_size_trait<Enum>::size())), rit(it(0)));
}

template <typename Enum>
boost::iterator_range<enum_iterator<Enum>> enum_range_from(Enum e) {
    typedef enum_iterator<Enum> it;
    return boost::make_iterator_range(it(e), it());
}

template <typename Enum>
boost::iterator_range<boost::reverse_iterator<enum_iterator<Enum>>> reverse_enum_range_from(Enum e) {
    typedef enum_iterator<Enum> it;
    typedef boost::reverse_iterator<it> rit;
    using e_type = typename get_enum_type<Enum>::type;
    return boost::make_iterator_range(rit(it(static_cast<Enum>(static_cast<e_type>(e) + 1))), rit(it(0)));
}

template <typename EnumKey, typename Value>
class flat_enum_map_iterator : public boost::iterator_facade<flat_enum_map_iterator<EnumKey, Value>,
                                                             std::pair<EnumKey, Value&>,
                                                             boost::random_access_traversal_tag,
                                                             std::pair<EnumKey, Value&>> {
    typedef flat_enum_map<EnumKey, Value> enum_map;
    typedef typename flat_enum_map_iterator<EnumKey, Value>::difference_type difference_type;
    typename enum_map::underlying_container::iterator _iterator;
    enum_iterator<EnumKey> _enum_iterator;

public:
    flat_enum_map_iterator() {}
    flat_enum_map_iterator(typename enum_map::underlying_container::iterator it) : _iterator(it) {}
    flat_enum_map_iterator(typename enum_map::underlying_container::iterator it, EnumKey e)
        : _iterator(it), _enum_iterator(e) {}

    void increment() {
        ++_iterator;
        ++_enum_iterator;
    }

    void decrement() {
        --_iterator;
        --_enum_iterator;
    }

    void advance(difference_type n) {
        _iterator += n;
        _enum_iterator += n;
    }

    difference_type distance_to(const flat_enum_map_iterator& other) { return this->_iterator - other._iterator; }

    bool equal(const flat_enum_map_iterator& other) const { return this->_iterator == other._iterator; }

    std::pair<EnumKey, Value&> dereference() const { return {*_enum_iterator, *_iterator}; }
};

}  // namespace navitia
