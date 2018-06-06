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

#include <boost/range/algorithm/count_if.hpp>
#include <iterator>
#include <functional>
#include <stdexcept>

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
 *      // pair.first and pair.second are iterators
 *      // to the container `s`
 *       pairs.push_back({*pair.first, *pair.second});
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
        It end;

    public:
        PairsGeneratorIterator(It it, It sub_it, It end):
            it(it),
            sub_it(sub_it),
            end(end)
        {}

        std::pair<It, It> operator*() const {
            return std::make_pair( it, sub_it);
        }

        PairsGeneratorIterator& operator++() {

            std::advance(sub_it, 1);
            if(sub_it == end) {

                std::advance(it, 1);
                auto next_it = std::next(it);
                if( next_it == end ) {
                    /// We stop before the end of the container (ie 'end-1')
                    /// as we need at least 2 elements to make a pair
                    /// and at this point we only have one.
                    it = sub_it = end;
                    return *this;
                }

                sub_it = next_it;
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

    PairsGenerator(It beg, It end): _begin(beg), _end(end) {
        if( _begin == _end )
            throw std::invalid_argument("Container should not be empty");
    }

    PairsGeneratorIterator begin() const {
        auto next_it = std::next(_begin);
        if( next_it == _end) {
            /// we need at least 2 elements to make a pair
            /// and we only have one...
            return end();
        }

        return PairsGeneratorIterator(_begin, next_it, _end);
    }

    PairsGeneratorIterator end() const {
        return PairsGeneratorIterator(_end, _end, _end);
    }

private:
    It _begin;
    It _end;
};

template<class Container>
PairsGenerator<Container> make_pairs_generator(const Container & container)
{
    return PairsGenerator<Container>(container.cbegin(), container.cend());
}

template<class Container, class Op>
std::vector<typename Container::const_iterator> pairs_generator_unique_iterators(
    const Container & container,
    Op on_pair)
{
    typedef typename Container::const_iterator const_itr;

    std::vector<const_itr> visited_iterators;

    auto pairs_gen = make_pairs_generator(container);
    for(auto pair : pairs_gen) {

        auto already_visited = boost::count_if(visited_iterators,
            [&](const const_itr& it) {
                return it == pair.first || it == pair.second;
        });

        if (already_visited)
            continue;

        auto& pair1 = *pair.first;
        auto& pair2 = *pair.second;

        if(on_pair(pair1, pair2)) {
            visited_iterators.push_back(pair.first);
        }
        else if(on_pair(pair2, pair1)) {
            visited_iterators.push_back(pair.second);
        }
    }

    return visited_iterators;
}

} // namespace utils
} // namespace navitia
