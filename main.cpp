#include "data_task.h"

int main(int argc, char*argv[]) {
    data_task d(argv[1]);

    try {
        d.add_new_task("12.03.2024", "learn algorithm", "15:50", "16:59");
        d.show_tasks_for_the_current_day("12.03.2024");
        d.print_data();
    } catch(const char* e) {
        std::cout << e << std::endl;
    }
    d.write_to_json();


    return 0;
}