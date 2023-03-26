#include "data_task.h"

int main(int argc, char*argv[]) {
    data_task d(argv[1]);

    try {
        d.print_data();
        d.reschedule_the_event("27.04.2023", 1, "28.04.2023", "23:00", "23:59");
        //d.remove("11.03.2023", 2);
        //d.remove("11.03.2023", 1);
        //d.add_new_task("12.03.2024", "learn c++", "11:42", "11:59");
        
    } catch(const char* e) {
        std::cout << e << std::endl;
    }
    d.write_to_json();
 

    return 0;
}