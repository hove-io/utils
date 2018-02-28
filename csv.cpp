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
#include "functions.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/foreach.hpp>

void CsvReader::init(){
    // Our parser will fail (and thus throw) if a quoted string is not
    // finished (by using > instead of >> that disallow backtrack).
    // Like that, if the parser throw, we ask for more input to read
    // the next line
    this->quoted_string = (qi::omit[(*qi::space)] >> // skipping spaces before
                           qi::lit('"')) >
                         *(
                             '\\' >> qi::char_('"') // \" as "
                             | '"' >> qi::char_('"') // or "" as "
                             | (qi::char_ - '"') // or not(")
                             ) >
                         qi::lit('"') >
                         qi::omit[(*qi::space)]; // skipping spaces after
    this->valid_characters = qi::char_ - '"' - this->separator - '\n';
    this->item = this->quoted_string | *this->valid_characters;
    this->csv_parser = this->item % separator;
}

CsvReader::CsvReader(const std::string& filename, char separator, bool read_headers, bool to_lower_headers, std::string encoding): filename(filename),
    file(), separator(separator), closed(false)
#ifdef HAVE_ICONV_H
    , converter(nullptr)
#endif
{
    this->init();
    file.open(filename, std::fstream::in);
    stream = new std::istream(file.rdbuf());
    stream->setstate(file.rdstate());
    if(encoding != "UTF-8"){
        //TODO la taille en dur s'mal
#ifdef HAVE_ICONV_H
        converter = std::make_unique<EncodingConverter>(encoding, "UTF-8", 2048);
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
    file() , separator(separator), closed(false)
#ifdef HAVE_ICONV_H
    , converter(nullptr)
#endif
{
    this->init();
    stream = new std::istream(sstream.rdbuf());
    if(encoding != "UTF-8"){
        //TODO la taille en dur s'mal
#ifdef HAVE_ICONV_H
        converter = std::make_unique<EncodingConverter>(encoding, "UTF-8", 2048);
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

bool CsvReader::is_open() const {
    return stream->good();
}

bool CsvReader::validate(const std::vector<std::string> &mandatory_headers) const {
    BOOST_FOREACH(auto header, mandatory_headers){
        if(headers.find(header) == headers.end())
            return false;
	}
    return true;
}

std::string CsvReader::missing_headers(const std::vector<std::string> &mandatory_headers) const {
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
        closed = true;
    }
}

bool CsvReader::eof() const{
    return stream->eof() | stream->bad() | stream->fail();
}

CsvReader::~CsvReader(){
    this->close();
}

std::string CsvReader::convert(const std::string& st) const {
#ifdef HAVE_ICONV_H
    if(converter != nullptr){
        return converter->convert(st);
    }
#endif
    return st;
}

std::pair<CsvReader::ParseStatus, std::vector<std::string>>
CsvReader::get_line(const std::string& str) const
{
    if (str.empty()) { return {ParseStatus::OK, {}}; }

    std::vector<std::string> vec;
    std::string::const_iterator s_begin = str.begin();
    std::string::const_iterator s_end = str.end();

    bool result = false;
    try {
        result = boost::spirit::qi::parse(s_begin, s_end, this->csv_parser, vec);
    } catch (qi::expectation_failure<std::string::const_iterator>& e) {
        // if we have an expectation failure at the end of the string,
        // then we need the next line to complete
        if (e.first == s_end) {
            return {ParseStatus::CONTINUE, {}};
        } else {
            return {ParseStatus::FAIL, {}};
        }
    }

    if (! result || s_begin != s_end) {
        return {ParseStatus::FAIL, {}};
    }
    for(auto& elt: vec) {
        elt = this->convert(elt);
        boost::trim(elt);
    }
    return {ParseStatus::OK, vec};
}

std::vector<std::string> CsvReader::next(){
    if(!is_open()){
        throw navitia::exception("file not open");
    }
    std::string temp;
    std::string line;
    std::pair<ParseStatus, std::vector<std::string>> status_vec;
    auto log = log4cplus::Logger::getInstance("log");

    while (true) {
        if(eof()){
            if (!line.empty()){
                LOG4CPLUS_WARN(log ,"Impossible to parse line: " << line);
            }
            return {};
        }
        std::getline(*stream, temp);
        if (line.empty()){
            line = temp;
        }else{
            if (!temp.empty()){
                line += '\n'; line += temp;
            }
        }
        status_vec = get_line(line);
        switch (status_vec.first) {
        case ParseStatus::OK: return status_vec.second;
        case ParseStatus::FAIL:
            LOG4CPLUS_WARN(log ,"Impossible to parse line: " << line);
            return {};
        case ParseStatus::CONTINUE: break;
        }
    }
}

int CsvReader::get_pos_col(const std::string & str) const {
    auto it = headers.find(str);
    if (it != headers.end())
        return it->second;
    return -1;
}

bool CsvReader::has_col(int col_idx, const std::vector<std::string>& row) const {
    return col_idx >= 0 && static_cast<size_t>(col_idx) < row.size();
}

bool CsvReader::is_valid(int col_idx, const std::vector<std::string>& row) const {
    return (has_col(col_idx, row) && (!row[col_idx].empty()));
}


void remove_bom(std::fstream& stream){
    char buffer[3];
    stream.read(buffer, 3);
    if(stream.gcount() != 3)
        return;
    if(buffer[0] == '\xEF' && buffer[1] == '\xBB'){
        //BOM UTF8
        return;
    }
    // no match with the BOM, we put back the char read
    for(int i=0; i<3; i++){
        stream.unget();
    }
}

/**
 * @brief check if row is empty
 * Avoid to process empty row and row with only carriage return characters.
 *
 * @return true if row is empty
 */
bool CsvReader::row_is_empty(const std::vector<std::string>& row) {
    if (row.empty() || (row.size() == 1 && row[0].empty())) {
        return true;
    }
    else {
        return false;
    }
}

