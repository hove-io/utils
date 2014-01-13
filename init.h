#pragma once
#include "logger.h"
#include <cxxabi.h>
#include <execinfo.h>
#include <csignal>

/**
 * Provide facilities to initialize an app
 *
 * Default initialize loggers
 * Initialize signal handling
 */

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


void print_backtrace()
{
    //from so
    //http://stackoverflow.com/questions/19190273/how-to-print-call-stack-in-c-c-more-beautifully
    void* addresses[256];
    const int n = backtrace(addresses, std::extent<decltype(addresses)>::value);
    const std::unique_ptr<char*, decltype(&std::free)> symbols(backtrace_symbols(addresses, n), &std::free);

    for (int i = 0; i < n; ++i) {
        std::stringstream ss;
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
        LOG4CPLUS_ERROR(log4cplus::Logger::getInstance("Logger"), ss.str());
    }
}

namespace {
void before_dying(int signum) {
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("logger"));
    LOG4CPLUS_FATAL(logger, "We reveived signal: " << signum << ", so it's time to die!!");

    navitia::print_backtrace();

    signal(signum, SIG_DFL);
    kill(getpid(), signum); //kill the process to enable the core generation
}
}

void init_signal_handling() {
    //    signal(SIGINT, stop);
    //    signal(SIGTERM, stop);

    signal(SIGPIPE, before_dying);
    signal(SIGABRT, before_dying);
    signal(SIGSEGV, before_dying);
    signal(SIGFPE, before_dying);
    signal(SIGILL, before_dying);
}

void init_app() {
    init_logger();

    init_signal_handling();
}
}
