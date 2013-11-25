#pragma once
#include <array>
#include <type_traits>

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

/**
 * Simple container associating an enum value to a value
 *
 * the underlying type is a std::array for performance concern
 */
template <typename EnumKey, typename Value>
class flat_enum_map {
    std::array<Value, enum_size_trait<EnumKey>::size()> _array;

public:
    Value& operator[] (EnumKey key) {
        return _array[static_cast<typename get_enum_type<EnumKey>::type>(key)];
    }
    const Value& operator[] (EnumKey key) const {
        return _array[static_cast<typename get_enum_type<EnumKey>::type>(key)];
    }

    //TODO if needed : forward arg for constructor
    //TODO if needed : create map like iterator (key value pair)
};

}
