#pragma once

#include <string>
#include <exception>
#include "backtrace.h"

namespace navitia {
class exception : public std::exception {
protected:
    std::string msg;
    std::string _backtrace;

public:
    exception(const std::string& msg): msg(msg), _backtrace(get_backtrace()){}
    exception() = default;

    const char* what() const noexcept {
        return msg.c_str();
    }
    const std::string& backtrace() const noexcept {
        return _backtrace;
    }

    virtual ~exception() noexcept {}
};

/**
 * non fatal exception
 *
 * Used in kraken for exemple if the worker can continue to work after having caught the exception
 *
 */
struct recoverable_exception : public exception
{
    recoverable_exception(const std::string& msg): exception(msg) {}
    recoverable_exception() = default;
};
}

