#include "data_task.h"

int main(int argc, char*argv[]) {
    data_task d(argv[1]);
    d.print_data();
    d.write_to_json();


    return 0;
}