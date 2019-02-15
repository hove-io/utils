/* Copyright © 2001-2015, Canal TP and/or its affiliates. All rights reserved.

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

#include "utils/obj_factory.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE obj_factory_test
#include <boost/test/unit_test.hpp>
#include "utils/functions.h"

struct HeaderInt {
    uint32_t idx;
    std::string uri;
    int val = 0;
};

BOOST_AUTO_TEST_CASE(simple_obj_factory) {
    auto i_int = HeaderInt();
    i_int.val = 2;
    auto iInt_bis = i_int;
    navitia::ObjFactory<HeaderInt> obj_factory;

    obj_factory.emplace("Hubert"); //name dropping
    BOOST_CHECK_EQUAL(obj_factory["Hubert"]->val, 0);
    obj_factory.get_or_create("Hubert", i_int); //should not change value
    BOOST_CHECK_EQUAL(obj_factory[navitia::Idx<HeaderInt>(0)]->val, 0);
    BOOST_CHECK_EQUAL(obj_factory.size(), 1);
    obj_factory.get_or_create("Hubert")->val = 1; //should change value
    BOOST_CHECK_EQUAL(obj_factory.get_mut(navitia::Idx<HeaderInt>(0))->val, 1);

    obj_factory.insert("John", std::move(i_int));
    BOOST_CHECK_EQUAL(obj_factory.get_mut("John")->val, 2);

    obj_factory.emplace("JohnEmpty");
    obj_factory.get_or_create("Bob", HeaderInt());
    obj_factory.get_or_create("BobEmpty");
    obj_factory.insert("OpenData", std::move(iInt_bis));
    obj_factory.get_mut("OpenData")->val = 4;
    BOOST_CHECK_EQUAL(obj_factory["OpenData"]->val, 4);
    BOOST_CHECK_EQUAL(obj_factory.size(), 6);
}

struct HeaderUniquePtrInt {
    HeaderUniquePtrInt(int val) : val(std::make_unique<int>(val)) {}
    HeaderUniquePtrInt() : val(std::make_unique<int>(0)) {}
    HeaderUniquePtrInt(HeaderUniquePtrInt&&) = default;
    HeaderUniquePtrInt& operator=(HeaderUniquePtrInt&&) = default;
    uint32_t idx;
    std::string uri;
    std::unique_ptr<int> val;
};

BOOST_AUTO_TEST_CASE(non_copyable_obj_factory) {
    navitia::ObjFactory<HeaderUniquePtrInt> obj_factory;

    obj_factory.emplace("Hubert", HeaderUniquePtrInt(0)); //name dropping
    BOOST_CHECK_EQUAL(*(obj_factory["Hubert"]->val), 0);
    obj_factory.get_or_create("Hubert", 2); //should not change value
    BOOST_CHECK_EQUAL(*(obj_factory[navitia::Idx<HeaderUniquePtrInt>(0)]->val), 0);
    BOOST_CHECK_EQUAL(obj_factory.size(), 1);
    *(obj_factory.get_or_create("Hubert")->val) = 1; //should change value
    BOOST_CHECK_EQUAL(*(obj_factory.get_mut(navitia::Idx<HeaderUniquePtrInt>(0))->val), 1);

    auto i_int = HeaderUniquePtrInt(2);
    obj_factory.insert("John", std::move(i_int));
    BOOST_CHECK_EQUAL(*(obj_factory.get_mut("John")->val), 2);

    obj_factory.emplace("JohnEmpty");
    obj_factory.get_or_create("Bob", 0);
    obj_factory.get_or_create("BobEmpty");
    auto iInt_bis = HeaderUniquePtrInt(2);
    obj_factory.insert("OpenData", std::move(iInt_bis));
    *(obj_factory.get_mut("OpenData")->val) = 4;
    BOOST_CHECK_EQUAL(*(obj_factory["OpenData"]->val), 4);
    BOOST_CHECK_EQUAL(obj_factory.size(), 6);
}

BOOST_AUTO_TEST_CASE(erase_menber_into_obj_factory) {

    // Erase with uri key
    {
        for (size_t i = 0; i < 4; ++i) {

            navitia::ObjFactory<HeaderInt> obj_factory;

            std::array<std::string, 4> uris = {"Val_0", "Val_1", "Val_2", "Val_3"};
            std::array<size_t, 4> values = {0, 1, 2, 3};

            // Data
            for (size_t j = 0; j < 4; ++j) {
                auto data = HeaderInt();
                data.val = values[j];
                obj_factory.insert(uris[j], std::move(data));
            }
            BOOST_CHECK_EQUAL(obj_factory.size(), 4);
            for (size_t j = 0; j < obj_factory.size(); ++j) {
                BOOST_CHECK_EQUAL((obj_factory[uris[i]]->val), i);
            }

            // Erase with Idx
            // Be carefull, erase function shifts (<<) elems inside vector
            // It changes the idx for all shifted elems
            BOOST_CHECK_EQUAL(obj_factory.erase("Val_10"), false);
            BOOST_CHECK_EQUAL(obj_factory.erase(uris[i]), true);
            BOOST_CHECK_EQUAL(obj_factory.size(), 3);
            BOOST_CHECK_EQUAL(obj_factory.exist(uris[i]), false);
            for (size_t j = 0; j < obj_factory.size(); ++j) {
                if (j < i) {
                    BOOST_CHECK_EQUAL(obj_factory.get_mut(navitia::Idx<HeaderInt>(j))->val, j);
                }
                else {
                    BOOST_CHECK_EQUAL(obj_factory.get_mut(navitia::Idx<HeaderInt>(j))->val, j + 1);
                }
            }
        }
    }

    // Erase with idx
    {
        for (size_t i = 0; i < 4; ++i) {

            navitia::ObjFactory<HeaderInt> obj_factory;

            std::array<std::string, 4> uris = {"Val_0", "Val_1", "Val_2", "Val_3"};
            std::array<size_t, 4> values = {0, 1, 2, 3};

            // Data
            for (size_t j = 0; j < 4; ++j) {
                auto data = HeaderInt();
                data.val = values[j];
                obj_factory.insert(uris[j], std::move(data));
            }
            BOOST_CHECK_EQUAL(obj_factory.size(), 4);
            for (size_t j = 0; j < obj_factory.size(); ++j) {
                BOOST_CHECK_EQUAL((obj_factory[uris[i]]->val), i);
            }

            // Erase with Idx
            // Be carefull, erase function shifts elems inside vector
            BOOST_CHECK_EQUAL(obj_factory.erase(navitia::Idx<HeaderInt>(10)), false);
            BOOST_CHECK_EQUAL(obj_factory.erase(navitia::Idx<HeaderInt>(i)), true);
            BOOST_CHECK_EQUAL(obj_factory.size(), 3);
            BOOST_CHECK_EQUAL(obj_factory.exist(uris[i]), false);
            for (size_t j = 0; j < obj_factory.size(); ++j) {
                if (j < i) {
                    BOOST_CHECK_EQUAL(obj_factory.get_mut(navitia::Idx<HeaderInt>(j))->val, j);
                }
                else {
                    BOOST_CHECK_EQUAL(obj_factory.get_mut(navitia::Idx<HeaderInt>(j))->val, j + 1);
                }
            }
        }
    }
}

