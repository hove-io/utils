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
namespace navitia {
template<typename Container>
Container paginate(const Container &indexes,
                                  int count, int start_page) {
    Container response;
    if(count>=0 && start_page>=0) {
        uint32_t begin_i = start_page * count;
        uint32_t end_i = begin_i + count;
        if(begin_i < indexes.size()) {
            auto begin = indexes.begin() + begin_i;
            auto end = (end_i < indexes.size()) ? indexes.begin() + end_i :
                                                  indexes.end();
            return Container(begin, end);
        }
    }
    return Container{};
}

}
