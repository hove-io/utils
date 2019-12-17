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

#include <boost/format.hpp>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#ifdef HAVE_LOGGINGMACROS_H
#include <log4cplus/loggingmacros.h>
#endif

namespace navitia {

// logger pattern example : [18-09-10 17:02:17,186] [comment] [INFO ] [NDC] - message file.cpp:248
static const std::string console_pattern_ref1 = "[%D{%y-%m-%d %H:%M:%S,%q}] ";
static const std::string console_pattern_ref2 = "[%-5p] [%x] - %m %b:%L  %n";

// syslog pattern example : Sep 11 14:16:11 hostname binary_name: [comment] [PID] [INFO] [NDC] - message file.cpp:261
static const std::string syslog_pattern_ref = "[%i] [%p] [%x] - %m %b:%L  %n";

/**
 * @brief init simple navitia logger
 */
inline void init_logger() {
    log4cplus::BasicConfigurator config;
    config.configure();

    auto properties = log4cplus::helpers::Properties();
    properties.setProperty("log4cplus.rootLogger", "DEBUG, console");
    properties.setProperty("log4cplus.appender.console", "log4cplus::ConsoleAppender");
    properties.setProperty("log4cplus.appender.console.layout", "log4cplus::PatternLayout");
    properties.setProperty("log4cplus.appender.console.layout.ConversionPattern",
                           console_pattern_ref1 + console_pattern_ref2);

    log4cplus::PropertyConfigurator configurator(properties);
    configurator.configure();
}

/**
 * @brief init navitia logger
 *
 * @param name The application name
 * @param level The level of the logger (INFO, DEBUG, etc..)
 * @param active_local_syslog Active syslog redirection
 * @param pattern Overload pattern logger
 */
inline void init_logger(const std::string& name,
                        const std::string& level,
                        const bool active_local_syslog = false,
                        const std::string& comment = "",
                        const std::string& pattern = "") {
    std::string prefix = "";
    if (!comment.empty()) {
        prefix = prefix + "[" + comment + "] ";
    }

    auto syslog_pattern = prefix + syslog_pattern_ref;
    auto console_pattern = console_pattern_ref1 + prefix + console_pattern_ref2;
    if (!pattern.empty()) {
        syslog_pattern = prefix + pattern;
        console_pattern = prefix + pattern;
    }

    log4cplus::BasicConfigurator config;
    config.configure();

    auto properties = log4cplus::helpers::Properties();

    if (active_local_syslog) {
        properties.setProperty("log4cplus.rootLogger", (boost::format("%s, syslog") % level).str());
        properties.setProperty("log4cplus.appender.syslog", "log4cplus::SysLogAppender");
        properties.setProperty("log4cplus.appender.syslog.ident", name);
        properties.setProperty("log4cplus.appender.syslog.facility", "local7");
        properties.setProperty("log4cplus.appender.syslog.layout", "log4cplus::PatternLayout");
        properties.setProperty("log4cplus.appender.syslog.layout.ConversionPattern", syslog_pattern);
    } else {
        properties.setProperty("log4cplus.rootLogger", (boost::format("%s, console") % level).str());
        properties.setProperty("log4cplus.appender.console", "log4cplus::ConsoleAppender");
        properties.setProperty("log4cplus.appender.console.ident", name);
        properties.setProperty("log4cplus.appender.console.layout", "log4cplus::PatternLayout");
        properties.setProperty("log4cplus.appender.console.layout.ConversionPattern", console_pattern);
    }

    log4cplus::PropertyConfigurator configurator(properties);
    configurator.configure();
}

/**
 * @brief init navitia logger with a config file
 *
 * @param config_file The configuration file path
 */
inline void init_logger(const std::string& config_file) {
    log4cplus::PropertyConfigurator::doConfigure(config_file);
}

}  // namespace navitia
