#include "data_task.h"
#include "menu_task.h"
#include <boost/exception/all.hpp>
int main(int argc, char*argv[]) {
    try {
        menu_task::exec(argv[1]);
    } catch(const char* e) {
        std::cout << e << std::endl;
    } catch(boost::exception & exc) {
        
    }

    
    
 

    return 0;
}