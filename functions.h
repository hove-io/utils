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
#include <iostream>
#include <vector>
#include <memory>
#include<map>

double str_to_double(std::string);
int str_to_int(std::string str);

/**
  Cette fonction permet de recupérer une chaine qui se trouve à une position donnée
  */
std::vector< std::string > split_string(const std::string&,const std::string & );


/**
 * Returns the corresponding mapped const reference in the map, or the
 * default constructed mapped_type if there is no such element.
 */
template<typename Map>
const typename Map::mapped_type&
find_or_default(const typename Map::key_type& k, const Map& m) {
    typedef typename Map::mapped_type mapped_type;
    static const mapped_type default_value = mapped_type();
    const auto search = m.find(k);
    return search == m.end() ? default_value : search->second;
}

/**
  Cette fonction permet de recupérer une valeur par une clef à partir de std::map<key, value>
  */
std::string value_by_key(const std::map<std::string, std::string>& vect, const std::string& key);

/** Foncteur permettant de comparer les objets en passant des pointeurs vers ces objets */
struct Less{
    template<class T>
        bool operator() (T* x, T* y) const{
            return *x < *y;
        }
};
/**
 * tests if elt is in range range
 */
template<typename T, typename U>
bool in(const T& elt, std::initializer_list<U> range) {
    for (const auto& cur: range)
        if (elt == cur) return true;
    return false;
}

/** Foncteur fixe le membre "idx" d'un objet en incrémentant toujours de 1
 *
 * Cela permet de numéroter tous les objets de 0 à n-1 d'un vecteur de pointeurs
 */
template<typename idx_t>
struct Indexer{
    idx_t idx;
    Indexer(): idx(0){}

    template<class T>
        void operator()(T* obj){obj->idx = idx; idx++;}
};

/**
 * Adding a make_unique for unique_ptr construction to ease use and ensure better exception safety
 */
#if __cplusplus <= 201103L //the make_unique will be added in c++14
namespace std {
template <typename T, typename ...Args>
unique_ptr<T> make_unique(Args&& ...args) {
    return unique_ptr<T>(new T(forward<Args>(args)...));
}
}
#endif

namespace navitia {

/**
 * speudo natural sort:
 * if both string cary integer, we compare them, else we compare the string
 */
struct pseudo_natural_sort {
    bool operator() (const std::string&, const std::string&) const;
};

/**
 * sort_and_truncate:
 * Here we compare two structures on different attributswe compare the string
 * and truncate the list at position nbmax
 */
template <typename Vector, typename Cmp>
Vector sort_and_truncate(Vector input, size_t nbmax, Cmp cmp) {
    typename Vector::iterator middle_iterator;
    if (nbmax < input.size())
        middle_iterator = input.begin() + nbmax;
    else
        middle_iterator = input.end();
    std::partial_sort(input.begin(), middle_iterator, input.end(), cmp);
    if (input.size() > nbmax) input.resize(nbmax);
    return std::move(input);
}

}
