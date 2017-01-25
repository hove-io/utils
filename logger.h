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
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <boost/format.hpp>
#ifdef HAVE_LOGGINGMACROS_H
#include <log4cplus/loggingmacros.h>
#endif

/** Crée une configuration par défaut pour le logger */
inline void init_logger(){
    log4cplus::BasicConfigurator config;
    config.configure();
}

/** configure the logger with a level and a pattern, the output will be on stdout */
inline void init_logger(const std::string& level, const std::string& pattern){
    log4cplus::BasicConfigurator config;
    config.configure();
    auto properties = log4cplus::helpers::Properties();
    properties.setProperty("log4cplus.rootLogger", (boost::format("%s, default") % level).str());
    properties.setProperty("log4cplus.appender.default", "log4cplus::ConsoleAppender");
    properties.setProperty("log4cplus.appender.default.layout", "log4cplus::PatternLayout");
    properties.setProperty("log4cplus.appender.default.layout.ConversionPattern", pattern);
    log4cplus::PropertyConfigurator configurator(properties);
    configurator.configure();
}

/** Configure le logger à partir du fichier de configuration */
inline void init_logger(const std::string & config_file){
    log4cplus::PropertyConfigurator::doConfigure(config_file);
}
