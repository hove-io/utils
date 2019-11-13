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
#include "alphanum.hpp"
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

bool pseudo_natural_sort::operator() (const std::string& a, const std::string&b) const {
    return doj::alphanum_less<std::string>()(a, b);
}
std::string make_adapted_uri_fast(const std::string& ref_uri, size_t s) {
    return ref_uri + ":adapted-" + boost::lexical_cast<std::string>(s);
}

std::string make_adapted_uri(const std::string& ref_uri) {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::stringstream uuid_stream;
    uuid_stream << uuid;
    return ref_uri + ":adapted-" + uuid_stream.str();
}

std::string absolute_path(){
    char buf[256];
    if (getcwd(buf, sizeof(buf))) {
        return std::string(buf) + "/";
    } else {
        std::perror("getcwd");
        return "";
    }
}

std::string strip_accents(std::string str) {
    const static std::vector<std::pair<std::string, std::string> > vec_str {
        {"à", "a"}, {"À", "a"}, {"â", "a"}, {"Â", "a"}, {"ä", "a"},  {"Ä", "a"},  {"é", "e"},
        {"É", "e"}, {"è", "e"}, {"È", "e"}, {"ê", "e"}, {"Ê", "e"},  {"ë", "e"},  {"Ë", "e"},
        {"ô", "o"}, {"Ô", "o"}, {"ö", "o"}, {"Ö", "o"}, {"ò", "o"},  {"Ò", "o"},  {"û", "u"},
        {"Û", "u"}, {"ù", "u"}, {"Ù", "u"}, {"ü", "u"}, {"Ü", "u"},  {"ç", "c"},  {"Ç", "c"},
        {"ï", "i"}, {"Ï", "i"}, {"î", "i"}, {"Î", "i"}, {"œ", "oe"}, {"æ", "ae"}, {"’", "'"}};

    for (const auto& vec : vec_str) {
        boost::algorithm::replace_all(str, vec.first, vec.second);
    }
    return str;
}

std::string strip_accents_and_lower(const std::string& str) {
    return boost::to_lower_copy(strip_accents(str));
}

} // namespace navitia
