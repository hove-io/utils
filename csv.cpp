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

#include "csv.h"
#include "logger.h"
#include "exception.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>


typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

CsvReader::CsvReader(const std::string& filename, char separator, bool read_headers, bool to_lower_headers, std::string encoding): filename(filename),
    file(), closed(false), functor('\\', separator, '"')
#ifdef HAVE_ICONV_H
	, converter(NULL)
#endif
{
    file.open(filename);
    stream = new std::istream(file.rdbuf());
    stream->setstate(file.rdstate());
    if(encoding != "UTF-8"){
        //TODO la taille en dur s'mal
#ifdef HAVE_ICONV_H
        converter = new EncodingConverter(encoding, "UTF-8", 2048);
#endif
    }

    if(file.is_open()) {

        remove_bom(file);

        if(read_headers) {
            auto line = next();
            for(size_t i=0; i<line.size(); ++i){
	            if(to_lower_headers)
            		boost::to_lower(line[i]);
                this->headers.insert(std::make_pair(line[i], i));
            }
        }
    } else {
        closed = true;
    }
}

CsvReader::CsvReader(std::stringstream &sstream, char separator, bool read_headers, bool to_lower_headers, std::string encoding): filename("sstream"),
    file(), closed(false), functor('\\', separator, '"') 
#ifdef HAVE_ICONV_H
	, converter(NULL)
#endif
{
    stream = new std::istream(sstream.rdbuf());
    if(encoding != "UTF-8"){
        //TODO la taille en dur s'mal
#ifdef HAVE_ICONV_H
        converter = new EncodingConverter(encoding, "UTF-8", 2048);
#endif
    }

    if(read_headers) {
        auto line = next();
        for(size_t i=0; i<line.size(); ++i){
            if(to_lower_headers)
	            boost::to_lower(line[i]);
            this->headers.insert(std::make_pair(line[i], i));
        }
    } 
}

bool CsvReader::is_open() {
    return stream->good();
}

bool CsvReader::validate(const std::vector<std::string> &mandatory_headers) {
    BOOST_FOREACH(auto header, mandatory_headers){
        if(headers.find(header) == headers.end())
            return false;
	}
    return true;
}

std::string CsvReader::missing_headers(const std::vector<std::string> &mandatory_headers) {
    std::string result;
    BOOST_FOREACH(auto header, mandatory_headers){
        if(headers.find(header) == headers.end())
            result += header + ", ";
	}

    return result;

}

void CsvReader::close(){
    if(!closed){
        file.close();
#ifdef HAVE_ICONV_H
		//TODO handle with compil option and not with the platform
        if(converter != NULL) {
            delete converter;
            converter = NULL;
        }
#endif
        closed = true;
    }
}

bool CsvReader::eof() const{
    return stream->eof() | stream->bad() | stream->fail();
}

CsvReader::~CsvReader(){
    this->close();
}

std::vector<std::string> CsvReader::next(){
    if(!is_open()){
        throw navitia::exception("file not open");
    }

    do{
        if(eof()){
            return std::vector<std::string>();
        }
        std::getline(*stream, line);
    }while(line.empty());


#ifdef HAVE_ICONV_H
    if(converter != NULL){
        line = converter->convert(line);
    }
#endif

    boost::trim(line);
    std::vector<std::string> vec;
    try {
        Tokenizer tok(line, functor);
        vec.assign(tok.begin(), tok.end());
        BOOST_FOREACH(auto &s, vec)
            boost::trim(s);
    } catch(const std::runtime_error& e){
        LOG4CPLUS_WARN(log4cplus::Logger::getInstance("log") ,"Impossible to parse line: " << line
                       << " because of : " << e.what());
        return next();
    } catch(...){
        LOG4CPLUS_WARN(log4cplus::Logger::getInstance("log") ,"Impossible to parse line: " << line);
        return next();
    }

    return vec;
}

int CsvReader::get_pos_col(const std::string & str){
    auto it = headers.find(str);  /// Use when key does not exist

    if (it != headers.end())
        return headers[str];
    return -1;
}

bool CsvReader::has_col(int col_idx, const std::vector<std::string>& row){
    return col_idx >= 0 && static_cast<size_t>(col_idx) < row.size();
}

bool CsvReader::is_valid(int col_idx, const std::vector<std::string>& row){
    return (has_col(col_idx, row) && (!row[col_idx].empty()));
}


void remove_bom(std::fstream& stream){
    char buffer[3];
    stream.read(buffer, 3);
    if(stream.gcount() != 3)
        return;
    if(buffer[0] == (char)0xEF && buffer[1] == (char)0xBB){
        //BOM UTF8
        return;
    }
    // no match with the BOM, we put back the char read
    for(int i=0; i<3; i++){
        stream.unget();
    }
}
