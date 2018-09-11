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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE csvreader_test
#include <boost/test/unit_test.hpp>
#include "utils/logger.h"
#include "utils/init.h"
#include "utils/csv.h"

struct logger_initialized {
    logger_initialized()   { navitia::init_logger(); }

};
BOOST_GLOBAL_FIXTURE( logger_initialized );

/*
Slash between double quote, exemple:
    "AA BB\CC"
*/
BOOST_AUTO_TEST_CASE(test_slashs) {

    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << boost::algorithm::join(std::vector<std::string>({"12", "AA BB\\CC", "13"}), ";") << "\n";
    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "AA BB\\CC");
    BOOST_CHECK_EQUAL(result[2], "13");
}

/*
Slash and separator between double quote, exemple :
    "AA;BB\CC"
*/
BOOST_AUTO_TEST_CASE(slashs_separator) {

    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << boost::algorithm::join(std::vector<std::string>({"12", "\"AA;BB\\CC\"", "13"}), ";") << "\n";
    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "AA;BB\\CC");
    BOOST_CHECK_EQUAL(result[2], "13");
}

/*
\n between double quote, exemple :
    "AA BB
CC"
*/
BOOST_AUTO_TEST_CASE(return_line) {

    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << boost::algorithm::join(std::vector<std::string>({"12", "\"AA BB\nCC\"", "13"}), ";") << "\n";
    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "AA BB\nCC");
    BOOST_CHECK_EQUAL(result[2], "13");
}

/*
\n, separator and between double quote, exemple :
    "AA;BB
CC"
*/
BOOST_AUTO_TEST_CASE(return_line_separator) {

    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << boost::algorithm::join(std::vector<std::string>({"12", "\"AA;BB\nCC\"", "13"}), ";") << "\n";
    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "AA;BB\nCC");
    BOOST_CHECK_EQUAL(result[2], "13");
}

/*
\n and separator Slash between double quote, exemple :
    "AA;BB
CC\\DD"
*/
BOOST_AUTO_TEST_CASE(return_line_separator_slash) {

    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << boost::algorithm::join(std::vector<std::string>({"12", "\"AA;BB\nCC\\DD\"", "13"}), ";") << "\n";
    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "AA;BB\nCC\\DD");
    BOOST_CHECK_EQUAL(result[2], "13");
}

BOOST_AUTO_TEST_CASE(quoted_then_dos_newline) {
    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << "\"test\"\r\n";
    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(result[0], "test");
}

BOOST_AUTO_TEST_CASE(spaces_around_quoted) {
    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << " \"test1\" ;\"test2\" ; \"test3\"\n";
    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "test1");
    BOOST_CHECK_EQUAL(result[1], "test2");
    BOOST_CHECK_EQUAL(result[2], "test3");
}

BOOST_AUTO_TEST_CASE(quote_quote_in_quoted) {
    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << "\"\"\"12\";\"AA BB\"\"CC\";\"13\"\"\"\n";

    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "\"12");
    BOOST_CHECK_EQUAL(result[1], "AA BB\"CC");
    BOOST_CHECK_EQUAL(result[2], "13\"");
}

/*
\"n" between double quote, exemple :
    "AA BB\"CC"
*/
BOOST_AUTO_TEST_CASE(backslash_quote_in_quoted) {

    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << boost::algorithm::join(std::vector<std::string>({"\"\\\"12\"", "\"AA BB\\\"CC\"", "\"13\\\"\""}), ";") << "\n";

    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "\"12");
    BOOST_CHECK_EQUAL(result[1], "AA BB\"CC");
    BOOST_CHECK_EQUAL(result[2], "13\"");
}

BOOST_AUTO_TEST_CASE(empty_quoted_field) {

    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << "\"\";\"\";\"\"";

    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "");
    BOOST_CHECK_EQUAL(result[1], "");
    BOOST_CHECK_EQUAL(result[2], "");
}

/*
  can read a line after a failed line
*/
BOOST_AUTO_TEST_CASE(failed_line_then_correct_line) {

    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << "foo \"bar;titi\n";
    sstream << "toto;tata\n";

    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 0);
    result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 2);
    BOOST_CHECK_EQUAL(result[0], "toto");
    BOOST_CHECK_EQUAL(result[1], "tata");
}

BOOST_AUTO_TEST_CASE(parse_empty) {
    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    CsvReader csv(sstream);
    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 0);
}

/*
All tests
*/
BOOST_AUTO_TEST_CASE(all) {

    std::stringstream sstream(std::stringstream::in | std::stringstream::out);

    sstream << boost::algorithm::join(std::vector<std::string>({"12", "\"AA BB\\CC\"", "13"}), ";") << "\n";
    sstream << boost::algorithm::join(std::vector<std::string>({"12", "\"AA;BB\\CC\"", "13"}), ";") << "\n";
    sstream << boost::algorithm::join(std::vector<std::string>({"12", "\"AA BB\nCC\"", "13"}), ";") << "\n";
    sstream << boost::algorithm::join(std::vector<std::string>({"12", "\"AA;BB\nCC\\DD\"", "13"}), ";") << "\n";
    sstream << boost::algorithm::join(std::vector<std::string>({"12", "\"AA BB\\\"CC\"", "13"}), ";") << "\n";

    CsvReader csv(sstream);

    std::vector<std::string> result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "AA BB\\CC");
    BOOST_CHECK_EQUAL(result[2], "13");

    result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "AA;BB\\CC");
    BOOST_CHECK_EQUAL(result[2], "13");

    result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "AA BB\nCC");
    BOOST_CHECK_EQUAL(result[2], "13");

    result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "AA;BB\nCC\\DD");
    BOOST_CHECK_EQUAL(result[2], "13");

    result = csv.next();
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "AA BB\"CC");
    BOOST_CHECK_EQUAL(result[2], "13");

}
