#include "data_task.h"

int main(int argc, char*argv[]) {
    data_task d(argv[1]);
<<<<<<< HEAD
    try {
        d.print_data();
        d.add_new_task("12.03.2024", "learn c++", "11:42", "11:59");
        
    } catch(const char* e) {
        std::cout << e << std::endl;
    }
=======
    //d.add_new_task();
    d.print_data();
    d.remove_task();

    
>>>>>>> remotes/origin/branch_dev3
    d.write_to_json();


    return 0;
}