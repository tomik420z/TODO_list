#pragma once
#ifndef DATA_H
#define DATA_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/foreach.hpp>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <set>
#include "check_date.h"
#include "exception_data_task.h"

namespace property = boost::property_tree;
namespace calendar = boost::gregorian;

struct data_el {
    std::string start_time;
    std::string end_time;
    std::string task;
    size_t priority_lvl;

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
            try {
                calendar::date yymmdd = calendar::from_string(date);
            } catch(...) {
                throw exception_data_task::incorrect_format_date();
            }
            std::string start_time = node_values.get<std::string>("start_time"), 
                        end_time = node_values.get<std::string>("end_time"), 
                        task = node_values.get<std::string>("task");
            size_t priority_lvl = node_values.get<size_t>("priority_lvl");
            if (checking_time::check(start_time) && checking_time::check(end_time)) {
                set_data[date].emplace(data_el{std::move(start_time), std::move(end_time), std::move(task), priority_lvl});
            } else if (start_time >= end_time) {
                throw exception_data_task::incorrect_format_interval();
            } else {
                throw exception_data_task::incorrect_format_time();
            }
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

    void erase(std::set<data_el>& ref_set, std::set<data_el>::iterator it_erase, 
                                decltype(set_data)::iterator it_find) {
        ref_set.erase(it_erase);
        if (ref_set.empty()) {
            set_data.erase(it_find);
        }
        
    }

public:
    using container = decltype(set_data);
    data_task(const char* f_name) : file_name(f_name) {
        property::read_json(f_name, in_root);
        fill_set_data();
    }

    void write_to_json() {
        property::ptree list_tasks;
        for(auto& [key, values] : set_data) {
            for(auto& [start_time, end_time, c_task, priority_lvl] : values) {
                property::ptree node;
                node.put("date", std::move(key));
                node.put("start_time", std::move(start_time));
                node.put("end_time", std::move(end_time));
                node.put("task", std::move(c_task));
                node.put("priority_lvl", priority_lvl);
                list_tasks.push_back(std::make_pair("", node));
            }
        }

        out_root.add_child("tasks", list_tasks);
        property::write_json(file_name, out_root);
    }

    void add_new_task(std::string&& date, std::string&& task, std::string&& time_start, std::string&& time_end, size_t prioryty_lvl) {
        try {
            calendar::date yymmdd = calendar::from_string(date);
            }
        catch(...) {
            throw exception_data_task::incorrect_format_date();
        }
        if (!checking_time::check(time_start) || !checking_time::check(time_end)) {
            throw exception_data_task::incorrect_format_time();
        }
        if (time_start >= time_end) {
            throw exception_data_task::incorrect_format_interval();
        }

        if (auto it_find = set_data.find(date); it_find != set_data.end()) {
            auto& ref_set = it_find->second;
            
            auto iter_next = ref_set.upper_bound(data_el{time_start, std::string{}, std::string{}, size_t{}});
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
                ref_set.insert(data_el{std::move(time_start), std::move(time_end), std::move(task), prioryty_lvl});
            } else {
                throw exception_data_task::task_is_superimposed_on_another();
            }
             

        } else {
            set_data[date].insert(data_el{std::move(time_start), std::move(time_end), std::move(task), prioryty_lvl});  
        }
    }
    
    std::set<data_el>::iterator select(std::set<data_el>& ref_set, size_t select_index) {
        if (1 <= select_index && select_index <= ref_set.size()) {
            auto it_select = ref_set.begin();
            for(size_t i = 1; i < select_index; ++i) {
                ++it_select;
            }
            return it_select;
        } else {
            throw exception_data_task::missing_value_in_the_list(select_index);
        }
    } 

    void remove(const std::string& date, size_t select_index) {
        try {
            calendar::date yymmdd = calendar::from_string(date);
            }
        catch(...) {
            throw exception_data_task::incorrect_format_date();
        }
        if (auto it_find = set_data.find(date); it_find != set_data.end()) {
            auto& ref_set = it_find->second;
            auto it_erase = select(ref_set, select_index);
            erase(ref_set, it_erase, it_find);
        } else {
            throw exception_data_task::non_existent_date();
        }
    }

    void reschedule_the_event(const std::string& date, size_t select_index, 
                                std::string&& new_date, std::string&& new_start, std::string&& new_end) {
        try {
            calendar::date yymmdd = calendar::from_string(new_date);
            }
        catch(...) {
            throw exception_data_task::incorrect_format_date();
        }
        if (!checking_time::check(new_start) || !checking_time::check(new_end)) {
            throw exception_data_task::incorrect_format_time();
        }

        if (new_start >= new_end) {
            throw exception_data_task::incorrect_format_interval();
        }
        if (auto it_find = set_data.find(date); it_find != set_data.end()) {
            auto ref_set = it_find->second;
            auto it_erase = select(ref_set, select_index);
            std::string task = it_erase->task;
            size_t priority_lvl = it_erase->priority_lvl;
            erase(ref_set, it_erase, it_find);
            add_new_task(std::move(new_date), std::move(task), std::move(new_start), std::move(new_end), priority_lvl);
        } else {
            throw exception_data_task::non_existent_date();
        }
    } 

    ~data_task() {}

    void print_data() const {
        for(const auto& [key, value] : set_data) {
            std::cout << "date " << key  << std::endl;
            for(const auto& [start_time, end_time, c_task, prioryty_lvl] : value) {
                std::cout << start_time << "-" << end_time << " " << c_task  << " " << prioryty_lvl << std::endl;
            }
        }
    }

    void show_tasks_for_the_given_day(const std::string& date) {
        try {
            calendar::date yymmdd = calendar::from_string(date);
            }
        catch(...) {
            throw exception_data_task::incorrect_format_date();
        }
        std::cout << "date: " << date << std::endl;
        if (auto it = set_data.find(date); it != set_data.end()) {
            for (const auto& [c_time_start, c_time_end, c_task, priority_lvl] : it->second) {
                std::cout << "time: " << c_time_start << "-" << c_time_end  <<  std::endl;
                std::cout << "task: " << c_task << std::endl;
                std::cout << "priority: " << priority_lvl << std::endl;
            }
        } else {
            std::cout << "There are no scheduled tasks for this date" << std::endl;
        }
    }

    decltype(auto) choose_date(const std::string& date) {
        return set_data.find(date);
    }

    container::iterator begin() noexcept {
        return set_data.begin();
    }

    container::iterator end() noexcept {
        return set_data.end();
    }

    void print_data_set(const std::set<data_el> & set) {
        size_t ind = 0;
        for(const auto & [start_time, end_time, task, priority_lvl]  : set) {
            std::cout << ++ind << ". " << start_time << "-" << end_time << std::endl;
            std::cout << "task: " << task << std::endl;
            std::cout <<"priority: " << priority_lvl << std::endl;
        }
    }
};

#endif

