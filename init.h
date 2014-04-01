#pragma once
#include "logger.h"
#include "backtrace.h"
#include <csignal>
#include <unistd.h>


/**
 * Provide facilities to initialize an app
 *
 * Default initialize loggers
 * Initialize signal handling
 */

namespace navitia {


inline void print_backtrace() {
    LOG4CPLUS_ERROR(log4cplus::Logger::getInstance("Logger"), get_backtrace());
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

inline void init_signal_handling() {
    //    signal(SIGINT, stop);
    //    signal(SIGTERM, stop);

    signal(SIGPIPE, before_dying);
    signal(SIGABRT, before_dying);
    signal(SIGSEGV, before_dying);
    signal(SIGFPE, before_dying);
    signal(SIGILL, before_dying);
}

inline void init_app() {
    init_logger();

    init_signal_handling();
}
}
