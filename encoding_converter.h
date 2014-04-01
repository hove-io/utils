#pragma once
#include "config.h"
#ifdef HAVE_ICONV_H
#include <iconv.h>
#include <string>


/// Classe permettant de convertir l'encodage de chaînes de caractères
class EncodingConverter{
    public:
        EncodingConverter(std::string from, std::string to, size_t buffer_size);
        std::string convert(std::string& str);
        virtual ~EncodingConverter();
    private:
        iconv_t iconv_handler;
        char* iconv_input_buffer;
        char* iconv_output_buffer;
        size_t buffer_size;
};



#endif