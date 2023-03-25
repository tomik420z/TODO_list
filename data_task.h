#pragma once
#ifndef DATA_H
#define DATA_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <set>
#include "check_date.h"

namespace property = boost::property_tree;


struct data_el {
    std::string start_time;
    std::string end_time;
    std::string task;

    std::pair<std::string, std::string> get_time_interval() const {
        return std::pair{start_time, end_time};
    } 
    
    bool operator<(const data_el& rhs) const noexcept {
        return start_time < rhs.start_time;
    }
};


class data_task {
protected:
    using ptree_t = boost::property_tree::ptree;

    std::unordered_map<std::string, std::set<data_el>> set_data;

    ptree_t in_root;
    ptree_t out_root;

    const std::string file_name;

    void fill_set_data() {
        ptree_t list_nodes = in_root.get_child("tasks");

        for(const auto& [ingnore_path, node_values] : list_nodes) {
        
            std::string date = node_values.get<std::string>("date");
            std::string start_time = node_values.get<std::string>("start_time"), 
                        end_time = node_values.get<std::string>("end_time"), 
                        task = node_values.get<std::string>("task");
            if (checking_time::check(start_time) && checking_time::check(end_time)) {
                set_data[date].emplace(data_el{std::move(start_time), std::move(end_time), std::move(task)});
            } else {
                std::cout << "error time" << std::endl;
            }

        }
    }

    void print_data_set(const std::set<data_el> & set) {
        size_t ind = 0;
        for(const auto & [start_time, end_time, task]  : set) {
            std::cout << ++ind << ". " << start_time << "-" << end_time << std::endl;
            std::cout << "task: " << task << std::endl;
        }
    }

    bool check_interval(const std::string& start_interval, const std::string& end_interval, 
                        const std::string& new_start, const std::string& new_end) {
        if (new_end < start_interval) {
            return true;
        } else if (new_start > end_interval) {
            return true;
        } else {
            return false;
        }
    }

public:
    data_task(const char* f_name) : file_name(f_name) {
        property::read_json(f_name, in_root);
        fill_set_data();
    }

    void write_to_json() {
        property::ptree list_tasks;
        for(auto& [key, values] : set_data) {
            for(auto& [start_time, end_time, c_task] : values) {
                property::ptree node;
                node.put("date", std::move(key));
                node.put("start_time", std::move(start_time));
                node.put("end_time", std::move(end_time));
                node.put("task", std::move(c_task));
                list_tasks.push_back(std::make_pair("", node));
            }
        }

        out_root.add_child("tasks", list_tasks);
        property::write_json(file_name, out_root);
    }

    void add_new_task(std::string&& date, std::string&& task, std::string&& time_start, std::string&& time_end) {
        if (auto it_find = set_data.find(date); it_find != set_data.end()) {
            auto& ref_set = it_find->second;
            
            auto iter_next = ref_set.upper_bound(data_el{time_start, std::string{}, std::string{}});
            bool flag_insert = true;
            if (iter_next != ref_set.end()) {
                const auto& [next_start, next_end] = iter_next->get_time_interval();
                if (!check_interval(next_start, next_end, time_start, time_end)) {
                    flag_insert = false;
                }
            }

            if (auto iter_prev = iter_next; iter_next != ref_set.begin()) {
                --iter_prev;
                const auto &[prev_start, prev_end] = iter_prev->get_time_interval();
                if (!check_interval(prev_start, prev_end, time_start, time_end)) {
                    flag_insert = false;
                }
            }

            if (flag_insert) {
                ref_set.insert(data_el{std::move(time_start), std::move(time_end), std::move(task)});
            } else {
                throw "error";
            }
             

        } else {
            set_data[date].insert(data_el{std::move(time_start), std::move(time_end), std::move(task)});  
        }
    }
    
    void remove(const std::string& date, size_t select_index) {
        if (auto it_find = set_data.find(date); it_find != set_data.end()) {
            auto& ref_set = it_find->second;
            print_data_set(ref_set);
            if (1 <= select_index && select_index <= ref_set.size()) {
                auto it_erase = ref_set.begin();
                for(size_t i = 1; i < select_index; ++i) {
                    ++it_erase;
                }
                ref_set.erase(it_erase);
                if (ref_set.empty()) {
                    set_data.erase(it_find);
                }
            } else {
                throw "task with number n is not in the list"
            }
        } else {
            throw "there are no tasks for this date";
        }
    }
    
    ~data_task() {

    }

    void print_data() const {
        for(const auto& [key, value] : set_data) {
            std::cout << "date " << key  << std::endl;
            for(const auto& [start_time, end_time, c_task] : value) {
                std::cout << start_time << "-" << end_time << " " << c_task << std::endl;
            }
        }
    }
};

#endif

