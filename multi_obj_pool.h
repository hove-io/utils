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
#include <list>

template <typename Obj>
struct DefaultParetoFrontVisitor {
    void is_dominated_by(const Obj& /*to_insert*/, const Obj& /*front_cur*/){}
    void dominates(const Obj& /*to_insert*/, const Obj& /*front_cur*/){}
    void inserted(const Obj& /*to_insert*/){}
};

/*
 * Pareto front pool
 *
 * each solution in the pool is part of the pareto front
 *
 * the domination function takes 2 Obj (the solutions) and checks if
 * the first solution is dominated by the second
 */
template <typename Obj, typename Dominator, typename Visitor=DefaultParetoFrontVisitor<Obj>>
struct ParetoFront {
    typedef std::list<Obj> Pool;
    typedef typename Pool::value_type value_type;
    typedef typename Pool::const_iterator const_iterator;

    ParetoFront() = default;
    explicit ParetoFront(Dominator x): dominate(x) {}
    explicit ParetoFront(Dominator x, Visitor v): dominate(x), v(v) {}

    bool add(const Obj& obj);
    bool is_dominated(const Obj& obj);
    template <class Predicate>
    inline void remove_if (Predicate pred) { pareto_front.remove_if(std::move(pred)); }
    inline size_t size() const { return pareto_front.size(); }
    inline const_iterator begin() const { return pareto_front.begin(); }
    inline const_iterator end() const { return pareto_front.end(); }

    const Visitor& getV() const { return v; }

private:
    Pool pareto_front;
    Dominator dominate;
    Visitor v;
};

/*
 * Add the solution to the pool of best solution if is part of the pareto front
 *
 * return true if the solution has been added to the pareto front, false otherwise
 */
template <typename Obj, typename Dominator, typename Visitor>
bool ParetoFront<Obj, Dominator, Visitor>::add(const Obj& obj) {

    //we check if the new solution is dominated by one of the best
#ifndef NDEBUG
    bool is_best = false;
#endif

    for (auto it = pareto_front.begin(); it != pareto_front.end();) {
        const auto& cur = *it;
        if (dominate(cur, obj)) {
            // a solution cannot be the dominate one solution and be dominated by another
            // else it means the domination function is not well defined
            BOOST_ASSERT_MSG(! is_best, "MultiObjPool::add The Dominator is not correctly defined");
            v.is_dominated_by(obj, cur);
            return false;
        }
        // if the new solution dominate one solution from the pareto front, we need to remove this solution
        if (dominate(obj, cur)) {
#ifndef NDEBUG
            is_best = true;
#endif
            v.dominates(obj, cur);
            it = pareto_front.erase(it);
        } else {
            ++it;
        }
    }

    // the solution is not dominated by one of the best, we add it
    pareto_front.push_back(obj);
    v.inserted(obj);
    return true;
}

/*
 * Returns true if the given solution is dominated by one object of the pareto front
 */
template <typename Obj, typename Dominator, typename Visitor>
bool ParetoFront<Obj, Dominator, Visitor>::is_dominated(const Obj& obj) {
    for (const auto& cur : pareto_front) {
        if (dominate(cur, obj)) {
            return true;
        }
    }
    return false;
}
