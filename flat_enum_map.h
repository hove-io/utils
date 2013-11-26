#pragma once
#include <array>
#include <type_traits>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace navitia {

//template <typename EnumKey> struct enum_size_trait {};

//for the moment the compiler version does not support std::underlying type
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
 */
template <typename EnumKey, typename Value>
struct flat_enum_map {
    typedef std::array<Value, enum_size_trait<EnumKey>::size()> underlying_container;
    typedef flat_enum_map_iterator<EnumKey, Value> iterator;
    typedef flat_enum_map_iterator<EnumKey, const Value> const_iterator;

    underlying_container array;

    flat_enum_map() : array() {}

    Value& operator[] (EnumKey key) {
        return array[static_cast<typename get_enum_type<EnumKey>::type>(key)];
    }
    const Value& operator[] (EnumKey key) const {
        return array[static_cast<typename get_enum_type<EnumKey>::type>(key)];
    }

    template<class Archive> void serialize(Archive & ar, const unsigned int) {
        ar & boost::serialization::make_array(array.data(), array.size()); //default serialization not available in boost 1.48
    }

    const_iterator begin() const { return const_iterator(array.begin(), static_cast<EnumKey>(0)); }
    const_iterator end() const { return const_iterator(array.end()); }

    //TODO if needed : forward arg for constructor
};

template <typename EnumKey, typename Value>
class flat_enum_map_iterator : public boost::iterator_facade<flat_enum_map_iterator<EnumKey, Value>,
                                                            std::pair<EnumKey, Value>,
                                                            boost::random_access_traversal_tag,
                                                            std::pair<EnumKey, Value&>> {
    typedef flat_enum_map<EnumKey, Value> enum_map;
    typedef typename flat_enum_map_iterator<EnumKey, Value>::difference_type difference_type;
    typename enum_map::underlying_container::iterator _iterator;
    EnumKey _enumKey;
    EnumKey moveEnum(difference_type n) { return static_cast<EnumKey>(static_cast<typename get_enum_type<EnumKey>::type>(_enumKey) + n); }
public:
    flat_enum_map_iterator() {}
    flat_enum_map_iterator(typename enum_map::underlying_container::iterator it) : _iterator(it) {}
    flat_enum_map_iterator(typename enum_map::underlying_container::iterator it, EnumKey e) : _iterator(it), _enumKey(e) {}

    void increment() {
        ++_iterator;
        _enumKey = moveEnum(1);
    }

    void decrement() {
        --_iterator;
        _enumKey = moveEnum(-1);
    }

    void advance(difference_type n) {
        if ( n > 0 ) {
            _iterator += n;
        }
        else {
            _iterator -= n;
        }
        _enumKey = moveEnum(n);
    }
    difference_type distance_to(const flat_enum_map_iterator& other) {
        return this->_iterator - other._iterator;
    }

    bool equal(const flat_enum_map_iterator& other) const
    {
        return this->_iterator == other._iterator;
    }

    std::pair<EnumKey, Value&> dereference() const { return {_enumKey, *_iterator}; }

};

}
