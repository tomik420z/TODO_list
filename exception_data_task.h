#pragma once 
#ifndef EXCEPTION_DATA_TASK_H
#define EXCEPTION_DATA_TASK_H
#include <iostream>
#include <string>
class exception_data_task
{
private:
    
public:
    
    exception_data_task() {}

    static std::string incorrect_format_time() { return std::string("incorrect format time"); }

    static std::string incorrect_format_date() { return std::string("incorrect format date"); }

    static std::string incorrect_format_interval() { return std::string("incorrect format interval"); }

    static std::string missing_value_in_the_list(size_t index_select) { return std::string("task with number ") + 
                                                    std::to_string(index_select) + std::string(" is not in the list"); }
    
    static std::string non_existent_date() { std::string("there are no tasks for this date");  }

    static std::string task_is_superimposed_on_another() { std::string("Can't add a task because there is already a scheduled task at that time"); } 
    
    ~exception_data_task() {}
};



#endif
