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

#include "config.h"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#ifdef HAVE_ICONV_H
#include "encoding_converter.h"
#endif
#include <map>

/**
 * lecteur CSV basique, si iconv est disponible, le resultat serat retourné en UTF8
 */
class CsvReader {
    public:
        CsvReader(const std::string& filename, char separator=';', bool read_headers = false, bool to_lower_headers = false, std::string encoding="UTF-8");
        CsvReader(std::stringstream& sstream, char separator=';', bool read_headers = false, bool to_lower_headers = false, std::string encoding="UTF-8");

        ~CsvReader();
        std::vector<std::string> next();
        int get_pos_col(const std::string&);
        bool has_col(int col_idx, const std::vector<std::string>& row);
        bool is_valid(int col_idx, const std::vector<std::string>& row);
        bool eof() const;
        void close();
        bool is_open();
        bool validate(const std::vector<std::string> &mandatory_headers);
        std::string missing_headers(const std::vector<std::string> &mandatory_headers);
        std::string filename;
    private:


        std::string line;
        std::fstream file;
        std::stringstream sstream;
        std::istream *stream;
        bool closed;
        boost::escaped_list_separator<char> functor;
        std::unordered_map<std::string, int> headers;
#ifdef HAVE_ICONV_H
        EncodingConverter* converter;
#endif


};

/// Supprime le BOM s'il existe, il n'y a donc pas de risque à l'appeler tout seul
void remove_bom(std::fstream& stream);

