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

#include "functions.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <mutex>
#include <stdexcept>

namespace navitia {

// Encapsulate a unary function, and provide a least recently used
// cache.  The function must be pure (same argument => same result),
// and a Lru object must not be shared across threads.
template<typename F>
class Lru {
private:
    typedef typename boost::remove_cv<typename boost::remove_reference<
                                          typename F::argument_type>::type
                                      >::type key_type;
    typedef typename boost::remove_cv<typename boost::remove_reference<
                                          typename F::result_type>::type
                                      >::type mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;
    typedef boost::multi_index_container<
        value_type,
        boost::multi_index::indexed_by<
            boost::multi_index::sequenced<>,
            boost::multi_index::ordered_unique<
                boost::multi_index::member<value_type, const key_type, &value_type::first> > >
        > Cache;

    // the encapsulate function
    F f;

    // maximal cached values
    size_t max_cache;

    // the cache, mutable because side effect are not visible from the
    // exterior because of the purity of f
    mutable Cache cache;
    mutable size_t nb_cache_miss = 0;
    mutable size_t nb_calls = 0;

public:
    typedef mapped_type const& result_type;
    typedef typename F::argument_type argument_type;

    Lru(F fun, size_t max = 10): f(std::move(fun)), max_cache(max) {
        if (max < 1) {
            throw std::invalid_argument("max (size of cache) must be strictly positive");
        }
    }

    result_type operator()(argument_type arg) const {
        ++nb_calls;
        auto& list = cache.template get<0>();
        auto& map = cache.template get<1>();
        const auto search = map.find(arg);
        if (search != map.end()) {
            // put the cached value at the begining of the cache
            list.relocate(list.begin(), cache.template project<0>(search));
            return search->second;
        } else {
            ++nb_cache_miss;
            // insert the new value at the begining of the cache
            const auto ins = list.push_front(std::make_pair(arg, f(arg)));
            // clean the cache by the end (where the entries are the
            // older ones) until the requested size
            while (list.size() > max_cache) { list.pop_back(); }
            return ins.first->second;
        }
    }

    size_t get_nb_cache_miss() const { return nb_cache_miss; }
    size_t get_nb_calls() const { return nb_calls; }
};
template<typename F> inline Lru<F> make_lru(F fun, size_t max = 10) {
    return Lru<F>(std::move(fun), max);
}

template<typename F> struct ConcurrentLru {
private:
    struct SharedPtrF {
        F f;
        using argument_type = typename F::argument_type;
        using underlying_type =
            typename boost::remove_cv<
            typename boost::remove_reference<
                typename F::result_type>::type
            >::type const;
        using result_type = std::shared_ptr<underlying_type>;
        result_type operator()(argument_type arg) const {
            return std::make_shared<underlying_type>(f(arg));
        }
    };
    Lru<SharedPtrF> lru;
    std::unique_ptr<std::mutex> mutex = std::make_unique<std::mutex>();

public:
    using result_type = typename SharedPtrF::result_type;
    using argument_type = typename SharedPtrF::argument_type;

    ConcurrentLru(F fun, size_t max = 10): lru(SharedPtrF{std::move(fun)}, max) {}
    ConcurrentLru(ConcurrentLru&&) = default;// needed by old version of gcc

    result_type operator()(argument_type arg) const {
        std::lock_guard<std::mutex> lock(*mutex);
        return lru(arg);
    }

    size_t get_nb_cache_miss() const { return lru.get_nb_cache_miss(); }
    size_t get_nb_calls() const { return lru.get_nb_calls(); }
};
template<typename F> inline ConcurrentLru<F> make_concurrent_lru(F fun, size_t max = 10) {
    return ConcurrentLru<F>(std::move(fun), max);
}

} // namespace navitia
