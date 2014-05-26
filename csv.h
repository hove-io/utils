#pragma once

#include "config.h"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#ifdef HAVE_ICONV_H
#include "encoding_converter.h"
#endif
#include <map>
#include <boost/spirit/include/qi.hpp>
#include <boost/algorithm/string.hpp>

/**
 * lecteur CSV basique, si iconv est disponible, le resultat serat retourné en UTF8
 */
namespace qi = boost::spirit::qi;

class CsvReader {
    public:
        CsvReader(const std::string& filename, char separator=';', bool read_headers = false, bool to_lower_headers = false, std::string encoding="UTF-8");
        CsvReader(std::stringstream& sstream, char separator=';', bool read_headers = false, bool to_lower_headers = false, std::string encoding="UTF-8");

        ~CsvReader();
        std::vector<std::string> next();
        std::string getline();
        int get_pos_col(const std::string&);
        bool has_col(int col_idx, const std::vector<std::string>& row);
        bool is_valid(int col_idx, const std::vector<std::string>& row);
        bool eof() const;
        void close();
        bool is_open();
        bool validate(const std::vector<std::string> &mandatory_headers);
        std::string missing_headers(const std::vector<std::string> &mandatory_headers);
        std::string filename;
        std::vector<std::string> get_line(const std::string& str);
        std::string convert(std::string st);
        void init();
    private:

        qi::rule<std::string::const_iterator, std::string()> quoted_string;
        qi::rule<std::string::const_iterator, std::string()> valid_characters;
        qi::rule<std::string::const_iterator, std::string()> item ;
        qi::rule<std::string::const_iterator, std::vector<std::string>()> csv_parser;

        std::string line;
        std::fstream file;
        std::stringstream sstream;
        std::istream *stream;
        char separator;
        bool closed;
        std::unordered_map<std::string, int> headers;
#ifdef HAVE_ICONV_H
        EncodingConverter* converter;
#endif


};

/// Supprime le BOM s'il existe, il n'y a donc pas de risque à l'appeler tout seul
void remove_bom(std::fstream& stream);

