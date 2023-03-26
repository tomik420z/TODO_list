#include "data_task.h"
#include <boost/exception/all.hpp>
int main(int argc, char*argv[]) {
    
    

    try {
        data_task d(argv[1]);     
    } catch(const char* e) {
        std::cout << e << std::endl;
    } catch(boost::exception & exc) {
    
    }
    //d.write_to_json();
 

    return 0;
}