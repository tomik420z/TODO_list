#pragma once
#ifndef DATA_H
#define DATA_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <set>

struct data_el {
    std::string time;
    std::string task;
    
    bool operator<(const data_el& rhs) const noexcept {
        return time < rhs.time;
    }
};
    

class data_task {
protected:
    using ptree_t = boost::property_tree::ptree;
    std::unordered_map<std::string, std::set<data_el>> set_data;
public:
    data_task(const char* f_name) {
        
    }

    ~data_task() {

    }
};

#endif

