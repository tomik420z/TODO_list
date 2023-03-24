#include "data_task.h"

int main(int argc, char*argv[]) {
    data_task d(argv[1]);
    //d.add_new_task();
    d.print_data();
    d.remove_task();

    
    d.write_to_json();


    return 0;
}