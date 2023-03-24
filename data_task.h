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

namespace property = boost::property_tree;

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

    ptree_t in_root;
    ptree_t out_root;

    void fill_set_data() {
        ptree_t list_nodes = in_root.get_child("tasks");
        
        for(const auto& [ingnore_path, node_values] : list_nodes) {
        
            std::string date = node_values.get<std::string>("date");
            set_data[date].emplace(
                data_el{node_values.get<std::string>("time"), node_values.get<std::string>("task")}
                );            
        }
    }

public:
    data_task(const char* f_name) {
        property::read_json(f_name, in_root);
        fill_set_data();
    }
    

    
    ~data_task() {

    }

    void print_data() {
        for(const auto& [key, value] : set_data) {
            std::cout << "date " << key  << std::endl;
            for(const auto& [c_time, c_task] : value) {
                std::cout << c_time << " " << c_task << std::endl;
            }
        }
    }
};

#endif

