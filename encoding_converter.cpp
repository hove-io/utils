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

#include "encoding_converter.h"

#ifdef HAVE_ICONV_H
#include <string.h>
#include <fstream>

EncodingConverter::EncodingConverter(std::string from, std::string to, size_t buffer_size) : buffer_size(buffer_size){
    iconv_handler = iconv_open(to.c_str(), from.c_str());
    iconv_input_buffer = new char[buffer_size];
    iconv_output_buffer = new char[buffer_size];
}


std::string EncodingConverter::convert(const std::string& str){
    memset(iconv_output_buffer, 0, buffer_size);
    strncpy(iconv_input_buffer, str.c_str(), buffer_size);
    char* working_input = iconv_input_buffer;
    char* working_output = iconv_output_buffer;
    size_t output_left = buffer_size;
    size_t input_left = str.size();
    size_t result = iconv(iconv_handler, &working_input, &input_left, &working_output, &output_left);
    if(result == (size_t)-1){
        throw (std::string("iconv fail: ") + std::to_string(errno));
    }
    return std::string(iconv_output_buffer);

}


EncodingConverter::~EncodingConverter(){
    delete[] iconv_output_buffer;
    delete[] iconv_input_buffer;
    iconv_close(iconv_handler);

}


#endif
