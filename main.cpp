#include "data_task.h"
#include "menu_task.h"
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
int main() {
    try {
        menu_task::exec("dataset/tasks.json");
    } catch(...) {

    }

    return 0;
}