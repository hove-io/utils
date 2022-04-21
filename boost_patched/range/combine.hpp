/* Copyright © 2001-2021, Hove and/or its affiliates. All rights reserved.

This file is part of Navitia,
    the software to build cool stuff with public transport.

Hope you'll enjoy and contribute to this project,
    powered by Hove (www.hove.com).
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
channel `#navitia` on riot https://riot.im/app/#/room/#navitia:matrix.org
https://groups.google.com/d/forum/navitia
www.navitia.io
*/

#pragma once

#include <boost/range/combine.hpp>

/*
 * Before Boost 1.56, boost::combine was only supporting 2 parameters
 * This frees the parameters limitation to combine an arbitrary number of arguments.
 *
 *  So you can write :
 *      for( auto& z : boost::combine(vec1, vec2, vec3, ... ) )
 *          std::cout << get<0>(z) << get<1>(z) << get<2>(z) << ... ;
 *
 */
#if BOOST_VERSION < 105600

namespace boost {
namespace range {
    template<typename IterTuple>
    class combined_range: public iterator_range<zip_iterator<IterTuple> >
    {
        typedef iterator_range<zip_iterator<IterTuple> > base;
    public:
        combined_range(IterTuple first, IterTuple last)
            : base(first, last)
        {}
    };

    template<typename... Ranges>
    auto combine(Ranges&&... rngs) ->
        combined_range<decltype(boost::make_tuple(boost::begin(rngs)...))>
    {
        return combined_range<decltype(boost::make_tuple(boost::begin(rngs)...))>(
                    boost::make_tuple(boost::begin(rngs)...),
                    boost::make_tuple(boost::end(rngs)...));
    }

} // namespace range
using range::combine;
} // namespace boost

#endif
