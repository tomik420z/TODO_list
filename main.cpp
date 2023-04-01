#include "data_task.h"
#include "menu_task.h"
#include <boost/exception/all.hpp>
int main() {
    try {
        menu_task::exec("dataset/tasks.json");
    } catch(...) {

    }
   // boost::posix_time::ptime t(boost::posix_time::second_clock::local_time());
   // std::cout << t << std::endl;
    return 0;
}