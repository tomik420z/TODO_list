#include "data_task.h"
#include "menu_task.h"
#include <boost/exception/all.hpp>

int main() {
    try {
        menu_task::exec("dataset/tasks.json");
    } catch(...) {

    }
/*
    data_el dd;
    dd.start_time = "12:00";
    dd.end_time = "13:00";
    dd.task = "eeeeee";
    dd.priority_lvl = 3;
    dd.comments.emplace_back("122233");
    dd.comments.emplace_back("56778");
    write_binary("./dataset/tmp/.add/t.bin", dd, "2023-07-01");
*/ 
/*
    data_el dd;
    boost::filesystem::directory_iterator begin("./dataset/tmp/.add/");
    std::string date;
    read_binary(*begin, dd, date);
    std::cout << dd << std::endl;
    std::cout << date << std::endl;
*/
    return 0;
}