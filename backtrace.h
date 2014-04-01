#pragma once
#include <cxxabi.h>
#include <execinfo.h>
#include <string>
#include <memory>
#include <sstream>

namespace navitia {

namespace {

std::string demangle( const char* const symbol )
{
    const std::unique_ptr<char, decltype(&std::free)> demangled(
                abi::__cxa_demangle(symbol, 0, 0, 0), &std::free);
    if (demangled) {
        return demangled.get();
    }
    return symbol;
}
}

inline std::string get_backtrace()
{
    //from so
    //http://stackoverflow.com/questions/19190273/how-to-print-call-stack-in-c-c-more-beautifully
    void* addresses[256];
    const int n = backtrace(addresses, std::extent<decltype(addresses)>::value);
    const std::unique_ptr<char*, decltype(&std::free)> symbols(backtrace_symbols(addresses, n), &std::free);

    std::stringstream ss;
    for (int i = 0; i < n; ++i) {
        // we parse the symbols retrieved from backtrace_symbols() to
        // extract the "real" symbols that represent the mangled names.
        char* const symbol = symbols.get()[i];
        char* end = symbol;
        while(*end) {
            ++end;
        }
        // scanning is done backwards, since the module name
        // might contain both '+' or '(' characters.
        while(end != symbol && *end != '+') {
            --end;
        }
        char* begin = end;
        while(begin != symbol && *begin != '(') {
            --begin;
        }

        if (begin != symbol) {
            ss << std::string(symbol, ++begin - symbol);
            *end++ = '\0';
            ss << demangle(begin) << '+' << end;
        }
        else {
            ss << symbol;
        }
        ss << std::endl;
    }
    return ss.str();
}
}
