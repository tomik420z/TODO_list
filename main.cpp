#include "data_task.h"
#include "menu_task.h"
#include <boost/exception/all.hpp>

int main() {
    try {
        menu_task::exec("dataset/tasks.json");
    } catch(...) {

    }
    return 0;
}