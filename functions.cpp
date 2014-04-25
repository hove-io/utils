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

#include "functions.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>

double str_to_double(std::string str){
    boost::trim(str);
    try{
        boost::replace_all(str, ",", ".");
        return boost::lexical_cast<double>(str);
    } catch(...){        
        return -1;
    }
}

int str_to_int(std::string str){    
    boost::trim(str);
    if (str.empty()){
        return -1;
    }
    try{
        return boost::lexical_cast<int>(str);
    } catch(...){
        return -1;
    }
}

std::vector<std::string> split_string(const std::string& str,const std::string & separator){
    std::vector< std::string > SplitVec;
    split( SplitVec, str, boost::is_any_of(separator), boost::token_compress_on );
    return SplitVec;
}

std::string value_by_key(const std::map<std::string, std::string>& vect, const std::string& key){

    if (vect.find(key) != vect.end())
        return vect.at(key);
    return "";
}

namespace navitia {

/**
 * We don't try to do a real natural sort (too complicated for our purpose)
 * we only want to compare the strings as numbers only if they are castable to number
 */
bool pseudo_natural_sort::operator() (const std::string& a, const std::string&b) const {
    try {
        auto a_int = boost::lexical_cast<int>(a);
        auto b_int = boost::lexical_cast<int>(b);
        return a_int < b_int;
    } catch (boost::bad_lexical_cast) {
        return a < b;
    }
}
}
