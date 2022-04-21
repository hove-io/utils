/* Copyright © 2001-2014, Hove and/or its affiliates. All rights reserved.

This file is part of Navitia,
    the software to build cool stuff with public transport.

Hope you'll enjoy and contribute to this project,
    powered by Hove (www.hove.com).
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
#include <cxxabi.h>
#include <execinfo.h>
#include <string>
#include <memory>
#include <sstream>

namespace navitia {

namespace {

std::string demangle(const char* const symbol) {
    const std::unique_ptr<char, decltype(&std::free)> demangled(abi::__cxa_demangle(symbol, nullptr, nullptr, nullptr),
                                                                &std::free);
    if (demangled) {
        return demangled.get();
    }
    return symbol;
}
}  // namespace

inline std::string get_backtrace() {
    // from so
    // http://stackoverflow.com/questions/19190273/how-to-print-call-stack-in-c-c-more-beautifully
    void* addresses[256];
    const int n = backtrace(addresses, std::extent<decltype(addresses)>::value);
    const std::unique_ptr<char*, decltype(&std::free)> symbols(backtrace_symbols(addresses, n), &std::free);

    std::stringstream ss;
    for (int i = 0; i < n; ++i) {
        // we parse the symbols retrieved from backtrace_symbols() to
        // extract the "real" symbols that represent the mangled names.
        char* const symbol = symbols.get()[i];
        char* end = symbol;
        while (*end) {
            ++end;
        }
        // scanning is done backwards, since the module name
        // might contain both '+' or '(' characters.
        while (end != symbol && *end != '+') {
            --end;
        }
        char* begin = end;
        while (begin != symbol && *begin != '(') {
            --begin;
        }

        if (begin != symbol) {
            ss << std::string(symbol, size_t(++begin - symbol));
            *end++ = '\0';
            ss << demangle(begin) << '+' << end;
        } else {
            ss << symbol;
        }
        ss << std::endl;
    }
    return ss.str();
}
}  // namespace navitia
