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
        CsvReader(const std::string& filename, char separator=';', bool read_headers = false, std::string encoding="UTF-8");
        CsvReader(std::stringstream& sstream, char separator=';', bool read_headers = false, std::string encoding="UTF-8");

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

