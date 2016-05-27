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

#pragma once
#include "conf.h"
#include "logger.h"
#include "backtrace.h"
#include <csignal>
#include <unistd.h>
#include <iostream>


/**
 * Provide facilities to initialize an app
 *
 * Default initialize loggers
 * Initialize signal handling
 */

namespace navitia {


inline void print_backtrace() {
    LOG4CPLUS_ERROR(log4cplus::Logger::getInstance("Logger"), get_backtrace());

    //without the flush we might not have the error in the standard output
    std::cout.flush();
    std::cerr.flush();
}

namespace {
void before_dying(int signum) {
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("logger"));
    LOG4CPLUS_FATAL(logger, "We received signal: " << signum << ", so it's time to die!! version: "
                    << config::project_version);

    navitia::print_backtrace();

    signal(signum, SIG_DFL);
    kill(getpid(), signum); //kill the process to enable the core generation
}

void before_exit(int signum) {
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("logger"));
    LOG4CPLUS_INFO(logger, "Process terminated by signal: " << signum);
    signal(signum, SIG_DFL);
    exit(0);
}
}

inline void init_signal_handling() {
    signal(SIGPIPE, before_dying);
    signal(SIGABRT, before_dying);
    signal(SIGSEGV, before_dying);
    signal(SIGFPE, before_dying);
    signal(SIGILL, before_dying);

    signal(SIGTERM, before_exit);
    signal(SIGINT, before_exit);
}

inline void init_app() {
    init_logger();

    init_signal_handling();
}
}
