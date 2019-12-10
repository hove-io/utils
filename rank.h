/* Copyright © 2001-2019, Canal TP and/or its affiliates. All rights reserved.

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

#include <limits>
#include <ostream>

namespace navitia {

using rank_t = uint16_t;
const rank_t invalid_rank = std::numeric_limits<rank_t>::max();

// Strong typing of rank with a phantom type!
template <typename T>
struct Rank {
    inline explicit Rank() : val(invalid_rank) {}
    inline explicit Rank(const rank_t& v) : val(v) {}
    inline explicit Rank(const T& o) : val(o.order().val) {}
    inline bool is_valid() const { return val != invalid_rank; }
    inline bool operator==(const Rank& other) const { return val == other.val; }
    inline bool operator!=(const Rank& other) const { return val != other.val; }
    inline bool operator<(const Rank& other) const { return val < other.val; }
    inline bool operator<=(const Rank& other) const { return val <= other.val; }
    inline Rank<T> operator-(const rank_t& v) const { return Rank(val - v); }
    inline Rank<T>& operator--() {
        --val;
        return *this;
    }
    inline Rank<T> operator--(int) {
        Rank res(*this);
        --(*this);
        return res;
    }
    inline Rank<T> operator+(const rank_t& v) const { return Rank(val + v); }
    inline Rank<T>& operator++() {
        ++val;
        return *this;
    }
    inline Rank<T> operator++(int) {
        Rank res(*this);
        ++(*this);
        return res;
    }
    inline friend std::ostream& operator<<(std::ostream& os, const Rank& idx) { return os << idx.val; }

    template <class Archive>
    void serialize(Archive& ar, const unsigned int) {
        ar& val;
    }

    rank_t val;  // the value of the rank
};

}  // namespace navitia
