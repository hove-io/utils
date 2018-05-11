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

#include <functional>

namespace navitia {
namespace utils {

/**
 *    @brief Tools to generate all possible pairs (combination of 2) within a container
 *
 *    example:
 *
 *    ```
 *    vector<int> s {1, 2, 3, 4};
 *    vector<pair<int, int>> pairs;
 *
 *    auto pairs_gen = make_pairs_generator(s);
 *    for(auto pair : pairs_gen) {
 *        pairs.push_back(pair);
 *    }
 *    ```
 *
 *    In the example above, `pairs` will contain
 *    the following pairs:
 *        {1, 2}, {1, 3}, {1, 4},
 *        {2, 3}, {2, 4}, {3, 4}
 *    };
 */
template<class Container>
class PairsGenerator
{
public:

    typedef typename Container::const_iterator It;
    typedef typename Container::value_type T;

    class PairsGeneratorIterator
    {
        It it;
        It sub_it;
        It sub_end;

    public:
        PairsGeneratorIterator(It it, It sub_it, It sub_end):
            it(it),
            sub_it(sub_it),
            sub_end(sub_end)
        {}

        std::pair<const T&, const T&> operator*() const {
            return std::make_pair( std::cref(*it), std::cref(*sub_it));
        }

        PairsGeneratorIterator& operator++() {

            sub_it++;

            if(sub_it == sub_end) {
                it++;
                sub_it = it + 1;
            }

            return *this;
        }

        bool operator==(const PairsGeneratorIterator & rhs) const {
            return it == rhs.it;
        }

        bool operator!=(const PairsGeneratorIterator & rhs) const {
            return !(*this == rhs);
        }
    };

    PairsGenerator(It beg, It end): _begin(beg), _sub_begin(beg+1), _end(end) {
        if( _begin == _end )
            throw std::length_error("Container should not be empty");
    }

    PairsGeneratorIterator begin() const {
        return PairsGeneratorIterator(_begin, _sub_begin, _end);
    }

    PairsGeneratorIterator end() const {
        /// We stop before the end of the container (ie 'end-1') as we need 2 elements to make a pair
        /// and at this point we only have one.
        return PairsGeneratorIterator(_end-1, _end, _end);
    }

private:
    It _begin;
    It _sub_begin;
    It _end;
};

template<class Container>
PairsGenerator<Container> make_pairs_generator(const Container & container)
{
    return PairsGenerator<Container>(container.cbegin(), container.cend());
}

} // namespace utils
} // namespace navitia
