#include <iostream>
#include <unordered_set>
#include <boost/test/unit_test.hpp>
#include "../data_task.h"
using boost::unit_test::test_suite;
using namespace std;

void firstTest()
{
    unordered_map<string, set<data_el>> set_data1;

    //add_new_task
    set_data1["2023-04-28"].insert(data_el{"11:00", "12:00",  "chill", 3});

    data_task data_task_("tasks.json");

    BOOST_CHECK_NO_THROW(data_task_.add_new_task("2023-04-28", "chill", "11:00", "12:00", 3));

    auto set_data2 = data_task_.get_container();

    BOOST_CHECK(set_data1 == set_data2);

    //remove
    BOOST_CHECK_NO_THROW(data_task_.remove("2023-04-28", 1));

    set_data1.erase("2023-04-28");

    set_data2 = data_task_.get_container();

    BOOST_CHECK(set_data1 == set_data2);
}

test_suite *init_unit_test_suite(int, char *[])
{
    test_suite *test = BOOST_TEST_SUITE("Simple test");
    test->add(BOOST_TEST_CASE(&firstTest), 0);

    return test;
}