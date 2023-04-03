#pragma once
#ifndef DATA_H
#define DATA_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <set>
#include <map>
#include "check_date.h"
#include "exception_data_task.h"
#include <windows.h>


namespace property = boost::property_tree;
namespace calendar = boost::gregorian;

struct data_el {
    std::string start_time;
    std::string end_time;
    std::string task;
    size_t priority_lvl;
    std::vector<std::string> comments;
    bool is_save;
    std::pair<std::string, std::string> get_time_interval() const {
        return std::pair{start_time, end_time};
    } 
    
    bool operator<(const data_el& rhs) const noexcept {
        return start_time < rhs.start_time;
    }

    bool operator==(const data_el& rhs) const noexcept {
        return (start_time == rhs.start_time) && (end_time == rhs.end_time) && 
               (task == rhs.task) && (priority_lvl == rhs.priority_lvl);
    }

    static HANDLE output_handle;
};

HANDLE data_el::output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

std::ostream& operator<<(std::ostream& os, const data_el& task_node) {
    WORD clr;
    if (task_node.priority_lvl == 1) {
        clr = 02; 
    } else if (task_node.priority_lvl == 2) {
        clr = 06;
    } else if (task_node.priority_lvl == 3) {
        clr = 04;
    }

    SetConsoleTextAttribute(data_el::output_handle, clr);
    os <<  "task: " << task_node.task << std::endl;
    os << "time: " << task_node.start_time << "-" << task_node.end_time  << std::endl;
    os << "priority: ";
    if (task_node.priority_lvl == 1) {
        os << "low" << std::endl;
    } else if(task_node.priority_lvl == 2) {
        os << "medium" << std::endl;
    } else {
        os << "high" << std::endl;
    }

    if (!task_node.comments.empty()) {
        os << "comments: " << std::endl;
        for(const auto& comment : task_node.comments) {
            os << "** " << comment << " **" << std::endl;
        }
    }
    SetConsoleTextAttribute(data_el::output_handle, 07);
    return os;
}


void read_binary(const boost::filesystem::path& path, data_el & task, std::string& date) {
    auto&[start_time, end_time, task_data, priority_lvl, comments, flag_save] = task;
    boost::filesystem::ifstream buff(path, std::ios::in | std::ios::binary);
    char __bin_date[11];
    buff.read(__bin_date, sizeof(__bin_date) - 1);
    __bin_date[10] = '\0';
    char __bin_time_start[6];
    __bin_time_start[5] = '\0';
    buff.read(__bin_time_start, sizeof(__bin_time_start) - 1);
    char __bin_time_end[6];
    __bin_time_end[5] = '\0';
    buff.read(__bin_time_end, sizeof(__bin_time_end) - 1);

    size_t __sz_task;
    buff.read((char*)&__sz_task, sizeof(size_t));
    char* __task_bin = new char[__sz_task + 1];
    __task_bin[__sz_task] = '\0'; 
    buff.read((char*)__task_bin, __sz_task);
    size_t priority;
    buff.read((char*)&priority, sizeof(size_t));
    std::cout << priority << std::endl;
    size_t __sz_comment;
    buff.read((char*)&__sz_comment, sizeof(size_t));
    std::cout << __sz_comment << std::endl;
    for(size_t i = 0; i < __sz_comment; ++i) {
        size_t __sz_comm;
        buff.read((char*)&__sz_comm, sizeof(size_t));
        std::cout << __sz_comm << std::endl;
        char* __bin_comment = new char[__sz_comm + 1];
        __bin_comment[__sz_comm] = '\0';
        buff.read((char *)__bin_comment, __sz_comm);
        comments.emplace_back(__bin_comment);
        delete[] __bin_comment;
    }
    date = __bin_date;
    start_time = __bin_time_start;
    end_time = __bin_time_end;
    task_data = __task_bin;
    priority_lvl = priority;
    flag_save = false;
    delete[] __task_bin;
    buff.close();
}


void write_binary(const std::string& f_name, const data_el & task, const std::string& date) {
    const auto&[start_time, end_time, task_data, priority_lvl, comments, ignore_flag] = task;
    std::ofstream buff(f_name, std::ios::out | std::ios::binary);
    
    buff.write((char*)date.c_str(), date.size());
    buff.write((char*)start_time.c_str(), start_time.size());
    buff.write((char*)end_time.c_str(), end_time.size());
    size_t __sz_task = task_data.size();
    buff.write((char*)&__sz_task,  sizeof(size_t));
    
    buff.write((char*)task_data.c_str(), __sz_task);

    buff.write((char*)&priority_lvl, sizeof(size_t));
    size_t __sz_comment = comments.size();
    buff.write((char*)&__sz_comment, sizeof(size_t));
    for(size_t i = 0; i < __sz_comment; ++i) {
        size_t __sz_comm = comments[i].size();
        buff.write((char*)&__sz_comm, sizeof(size_t));
        buff.write((char *)comments[i].c_str(), __sz_comm);
    }
    buff.close();
}





class data_task {
public:
    using container = std::unordered_map<std::string, std::set<data_el>>;
protected:
    using ptree_t = boost::property_tree::ptree;
    std::map<std::string, std::set<data_el>*> set_date;
    std::unordered_map<std::string, std::set<data_el>> set_data;
    
    ptree_t in_root;
    bool saved_data;
    const std::string file_name;
    

    void __cleardir() {
        boost::filesystem::remove_all("./dataset/tmp/.add/");
        boost::filesystem::remove_all("./dataset/tmp/.remove/");
        boost::filesystem::create_directory("./dataset/tmp/.add/");
        boost::filesystem::create_directory("./dataset/tmp/.remove/");
    }



    void __read_dir_add(boost::filesystem::directory_iterator begin) {
        boost::filesystem::directory_iterator end;
        for(; begin != end; ++begin) {
            
            data_el save_node;
            std::string save_date;
            read_binary(*begin, save_node, save_date);
            
            if (auto it_hash = set_data.find(save_date); it_hash != set_data.end()) {
                // редактирование 
                if (auto it_insert = it_hash->second.find(save_node); it_insert != it_hash->second.end()) {
                    it_hash->second.erase(it_insert);
                }
            }
            set_data[save_date].insert(std::move(save_node));
            set_date[save_date] = &set_data[save_date];
            boost::filesystem::remove(*begin);
        }
    }

    void __read_dir_remove(boost::filesystem::directory_iterator begin) {
        boost::filesystem::directory_iterator end;
    
        for(; begin != end; ++begin) {
            
            data_el save_node;
            std::string save_date;
            read_binary(*begin, save_node, save_date);

            auto& ref_set = set_data[save_date];
            ref_set.erase(save_node);
            set_date[save_date] = &ref_set;
            
            if (ref_set.empty()) {
                set_date.erase(save_date);
                set_data.erase(save_date);
            }
            boost::filesystem::remove(*begin);
        }
    }


    void fill_tmp_data(boost::filesystem::directory_iterator dir_add_iter, boost::filesystem::directory_iterator dir_rm_iter) {
        __read_dir_add(dir_add_iter);
        __read_dir_remove(dir_rm_iter);
    }

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
            std::vector<std::string> comments;
            if (auto it = node_values.find("comment"); it != node_values.not_found()) { 
                auto values = node_values.get_child("comment");
                for(const auto &[path, val] : values) {
                    comments.push_back(val.data());
                }
            }

            if (checking_time::check(start_time) && checking_time::check(end_time)) {
                set_data[date].emplace(data_el{std::move(start_time), std::move(end_time), std::move(task), priority_lvl, std::move(comments), true});
                set_date[date] = &set_data[date];
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
                                decltype(set_data)::iterator it_find, const std::string& date) {
        auto& ref_time =  it_erase->start_time;
        char key_time[] = {ref_time[0], ref_time[1], ref_time[3], ref_time[4], '\0'};
        if (!it_erase->is_save) { // если задача не сохранена 
            boost::filesystem::remove(std::string("./dataset/tmp/.add/") + date + key_time + ".bin");
        } else {
            write_binary(std::string("./dataset/tmp/.remove/") + date + key_time + ".bin", *it_erase, date);
        }
        
        ref_set.erase(it_erase);
        if (ref_set.empty()) {
            set_date.erase(it_find->first);
            set_data.erase(it_find);
        }
    }

    template<bool flag_print>
    void print_dates(const calendar::date& date_first, const calendar::date& date_last) const {    
        
        auto it_begin = set_date.lower_bound(calendar::to_sql_string(date_first));
        auto it_end = set_date.upper_bound(calendar::to_sql_string(date_last));
        
        for(size_t i = 0; it_begin != it_end; ++i, ++it_begin) {
            if constexpr(flag_print) { 
                std::cout << "--------------------------------" << std::endl;
            }
            std::cout  << "DATE: " << it_begin->first << std::endl;
            if constexpr (flag_print) {
                print_data_set(*it_begin->second);
                std::cout << "--------------------------------" << std::endl;
            }
        }
    }
    
    void ins(std::string&& date, std::string&& task, std::string&& time_start, std::string&& time_end, size_t prioryty_lvl, std::vector<std::string> && comments,
            std::set<data_el>&ref_set) {

        char key_time[] = {time_start[0], time_start[1], time_start[3], time_start[4], '\0'};
        data_el new_node{std::move(time_start), std::move(time_end), std::move(task), prioryty_lvl, std::move(comments), false};
        
        if (boost::filesystem::exists(std::string("./dataset/tmp/.remove/") + date + std::string(key_time) + std::string(".bin"))) {
            boost::filesystem::remove(std::string("./dataset/tmp/.remove/") + date + std::string(key_time) + std::string(".bin"));
        }

        write_binary(std::string("./dataset/tmp/.add/") + date + std::string(key_time) + std::string(".bin") ,new_node, date);
    
        ref_set.insert(std::move(new_node));
        set_date[date] = &ref_set;
    }

public:
    data_task(const char* f_name) : file_name(f_name) {
        property::read_json(f_name, in_root);
        fill_set_data();
        if (!boost::filesystem::exists("./dataset/tmp/.add/")) {
            boost::filesystem::create_directory("./dataset/tmp/.add/");
        } 
        if (!boost::filesystem::exists("./dataset/tmp/.remove/")) {
            boost::filesystem::create_directory("./dataset/tmp/.remove/");
        }

        boost::filesystem::directory_iterator end;
        boost::filesystem::directory_iterator dir_add_iter("./dataset/tmp/.add/");
        boost::filesystem::directory_iterator dir_rm_iter("./dataset/tmp/.remove/");
        saved_data = !(dir_add_iter == end && dir_rm_iter == end);
    }

    void clear_tmp_dir() {
        __cleardir();
    }

    bool get_flag_saved() const noexcept {
        return saved_data;
    }

    void read_tmp_files() {
        boost::filesystem::directory_iterator dir_add_iter("./dataset/tmp/.add/");
        boost::filesystem::directory_iterator dir_rm_iter("./dataset/tmp/.remove/");
        __read_dir_add(dir_add_iter);
        __read_dir_remove(dir_rm_iter);
    }

    void write_to_json() {
        ptree_t out_root;
        property::ptree list_tasks;
        for(auto& [key, values] : set_data) {
            for(auto& [start_time, end_time, c_task, priority_lvl, comments, ignore_flag] : values) {
                property::ptree node;
                node.put("date", std::move(key));
                node.put("start_time", std::move(start_time));
                node.put("end_time", std::move(end_time));
                node.put("task", std::move(c_task));
                node.put("priority_lvl", priority_lvl);
                
                if (!comments.empty()) {          
                    property::ptree comments_node;
                    for(auto & comment : comments) {
                        property::ptree comment_node;
                        comment_node.put("", std::move(comment));
                        comments_node.push_back(std::make_pair("", comment_node));
                    }
                    node.add_child("comment", comments_node);
                }
                list_tasks.push_back(std::make_pair("", node));
            }
        }

        out_root.add_child("tasks", list_tasks);
        property::write_json(file_name, out_root);
        __cleardir();
    }


    void add_new_task(std::string&& date, std::string&& task, std::string&& time_start, std::string&& time_end, size_t prioryty_lvl, std::vector<std::string> && comments) {
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
        if(prioryty_lvl < 0 || prioryty_lvl > 3) {
            throw exception_data_task::missing_value_in_the_list(3);
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
                ins(std::move(date), std::move(task), std::move(time_start), std::move(time_end), prioryty_lvl, std::move(comments), ref_set);
            } else {
                throw exception_data_task::task_is_superimposed_on_another();
            }
             

        } else {
            ins(std::move(date), std::move(task), std::move(time_start), std::move(time_end), prioryty_lvl, std::move(comments), set_data[date]);
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
            erase(ref_set, it_erase, it_find, it_find->first);
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
            auto& ref_set = it_find->second;
            auto it_erase = select(ref_set, select_index);
            std::string task = std::move(it_erase->task);
            auto comments = std::move(it_erase->comments);
            size_t priority_lvl = it_erase->priority_lvl;
            erase(ref_set, it_erase, it_find, it_find->first);
            add_new_task(std::move(new_date), std::move(task), std::move(new_start), std::move(new_end), priority_lvl, std::move(comments));
        } else {
            throw exception_data_task::non_existent_date();
        }
    } 

    ~data_task() {}

    void print_data() const {
        for(const auto& [key, value] : set_data) {
            std::cout << "date " << key  << std::endl;
            for(const auto& [start_time, end_time, c_task, prioryty_lvl, comments, ignore_flag] : value) {
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
            size_t index = 0;
            std::cout << "---------------------------------" << std::endl;
            for (const auto& task : it->second) {
                std::cout << ++index << ". " << task << std::endl;
            }
            std::cout << "---------------------------------" << std::endl;
        } else {
            std::cout << "There are no scheduled tasks for this date" << std::endl;
        }
    }

    const container& get_container() const noexcept {
        return set_data;
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

    void print_data_set(const std::set<data_el> & set) const {
        size_t ind = 0;
        for(const auto& task  : set) {
            std::cout << ++ind << ". " << task << std::endl;
        }
    }

    void show_date(const std::string & date_first, const std::string& date_last) const {
        print_dates<false>(calendar::from_simple_string(date_first), calendar::from_simple_string(date_last));
    }

    void show_dates_with_tasks(const std::string & date_first, const std::string& date_last) const {
        print_dates<true>(calendar::from_simple_string(date_first), calendar::from_simple_string(date_last));
    }

};

#endif

