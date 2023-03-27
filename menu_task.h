#pragma once
#ifndef MENU_TASK_H
#define MENU_TASK_H
#include "data_task.h"
#include <windows.h>

class menu_task {
    static void print_menu() {
        std::cout << "MENU" << std::endl;
        std::cout << "--------------------------------" << std::endl;
        std::cout << "1. ADD NEW TASK" << std::endl;
        std::cout << "2. REMOVE TASK" << std::endl;
        std::cout << "3. RESCHEDULE THE EVENT" << std::endl;
        std::cout << "4. SHOW TASKS FOR THE GIVEN DAY" << std::endl;
        std::cout << "5. SHOW TASKS FOR TODAY" << std::endl;
        std::cout << "6. SAVE AND EXIT" << std::endl;
    }

    static std::string input_date() {
        std::string new_date;
        std::getline(std::cin, new_date, '\n');
        return new_date;
    }

    static std::string input_task() {
        std::string task;
        std::getline(std::cin, task, '\n');
        return task;
    }

    static std::string input_start_time() {
        std::string new_start;
        std::getline(std::cin, new_start, '\n');
        if (new_start.size() == 4) {
            new_start.insert(new_start.begin(), '0');
        }
        return new_start;
    }

    static std::string input_end_time() {
        std::string new_end;
        std::getline(std::cin, new_end, '\n');
        if (new_end.size() == 4) {
            new_end.insert(new_end.begin(), '0');
        }
        return new_end;
    }
    static void cin_get() {
        if (std::cin.peek() == '\n' || std::cin.peek() == ' ') {
            std::cin.get();
        }
    }

    static int parse_from_string(const std::string str) {
        int num = 0;
        for(auto ch : str) {
            if (isdigit(ch)) {
                num = num * 10 + (ch - '0');
            } else {
                throw std::string("you need to enter a number from 1 to 6");
            }
        }
        return num;

    } 


public:

    static void exec(const char* f_name) {
        
        HANDLE h;
        h = GetStdHandle(STD_OUTPUT_HANDLE); 
        data_task data(f_name);
        int select_index = 0;
        system("cls");
        print_menu();
        while(select_index != -1) {
            try {
                std::string str_num;
                std::cout << "select a number from 1 to 6" << std::endl;
                std::cin >> str_num;
                select_index = menu_task::parse_from_string(str_num);
                system("cls");
                print_menu();
                switch(select_index) {
                case 1: 
                    {
                        menu_task::cin_get();
                        std::cout << "enter task:"<< std::endl;
                        std::string new_task = input_task();
                        std::cout << "enter date:"<< std::endl;
                        std::string new_date = input_date();
                        std::cout << "enter start time:"<< std::endl;
                        std::string new_start = input_start_time();
                        std::cout << "enter final time:" << std::endl;
                        std::string new_end = input_end_time();
                        std::cout << "enter priority" << std::endl;
                        SetConsoleTextAttribute(h, 02);
                        std::cout << "1. low priority" << std::endl;
                        SetConsoleTextAttribute(h, 06);
                        std::cout << "2. medium priority" << std::endl;
                        SetConsoleTextAttribute(h, 04);
                        std::cout << "3. high priority" << std::endl;
                        SetConsoleTextAttribute(h, 07);
                        std::string str_select;
                        std::cout << "select a number from 1 to 3" << std::endl;
                        std::cin >> str_select;
                        size_t select_priority = menu_task::parse_from_string(str_select);
                        data.add_new_task(std::move(new_date), std::move(new_task), std::move(new_start), std::move(new_end), select_priority);
                        
                        system("cls");
                        print_menu();
                        
                        SetConsoleTextAttribute(h, 02);
                        std::cout << "task added successfully!" << std::endl;
                        SetConsoleTextAttribute(h, 07);

                        break;
                    }
                case 2:
                    {
                        menu_task::cin_get();
                        std::cout << "enter date:"<< std::endl;
                        std::string erase_date = input_date();
                        if (auto it_find = data.choose_date(erase_date); it_find != data.end()) {
                            auto& ref_set = it_find->second;
                            data.print_data_set(ref_set);
                            std::cout << ref_set.size() + 1 << ". ya peredumal udalat'" << std::endl;
                            std::string select_str;
                            size_t select_index = 0;
                            std::cout << "select number from " << 1 << " to " << ref_set.size() + 1 << ":" << std::endl;
                            std::cin >> select_str;
                            select_index = menu_task::parse_from_string(select_str);
                            if (select_index == ref_set.size() + 1) {
                                system("cls");
                                print_menu();
                                break;
                            }
                            menu_task::cin_get();
                            data.remove(erase_date, select_index);
                            
                            system("cls");
                            print_menu();
                            SetConsoleTextAttribute(h, 02);
                            std::cout << "task remove successfully!" << std::endl;
                            SetConsoleTextAttribute(h, 07);
                        } else {
                            throw std::string("no events for this date");
                        }
                        break;
                    }
                case 3:
                    {
                        menu_task::cin_get();
                        std::cout << "enter date:"<< std::endl;
                        std::string erase_date = input_date();
                        if (auto it_find = data.choose_date(erase_date); it_find != data.end()) {
                            auto& ref_set = it_find->second;
                            data.print_data_set(ref_set);
                            std::cout << ref_set.size() + 1 << ". ya peredumal perenosit'" << std::endl;
                            std::string select_str; 
                            size_t select_index = 0;
                            std::cout << "select number from " << 1 << " to " << ref_set.size() + 1 << ":" << std::endl;
                            std::cin >> select_str;
                            select_index = menu_task::parse_from_string(select_str);
                            if (select_index == ref_set.size() + 1) {
                                system("cls");
                                print_menu();
                                break;
                            }
                            menu_task::cin_get();
                            std::cout << "enter new date:" << std::endl;
                            std::string new_date = input_date();
                            std::cout << "enter new time start:" << std::endl;
                            std::string new_start = input_start_time();
                            std::cout << "input new time finish:" << std::endl; 
                            std::string new_end = input_end_time(); 
                            data.reschedule_the_event(erase_date, select_index, std::move(new_date), std::move(new_start), std::move(new_end));
                            
                            system("cls");
                            print_menu();
                            SetConsoleTextAttribute(h, 02);
                            std::cout << "task reschedule successfully!" << std::endl;
                            SetConsoleTextAttribute(h, 07);
                        } else {
                            throw std::string("no events for this date");
                        }
                        break;
                    }
                case 4: 
                    { 
                        menu_task::cin_get();
                        std::cout << "enter date:" << std::endl;
                        std::string input_date;
                        std::getline(std::cin, input_date, '\n');
                        data.show_tasks_for_the_given_day(input_date);
                        break;
                    }
                case 5:
                    {
                        data.show_tasks_for_the_given_day(calendar::to_sql_string(calendar::day_clock::local_day()));
                        break;
                    }
                case 6: 
                    {                
                        select_index = -1;
                        break;
                    }
                default:
                    {                
                        throw std::string("you need to enter a number from 1 to 6");
                        break;
                    }
                }
            } catch(const std::string& msg) {
                system("cls");
                print_menu();
                SetConsoleTextAttribute(h, 04);
                std::cout << "error: " << msg << std::endl;
                SetConsoleTextAttribute(h, 07);
            }
        } 

        data.write_to_json();
    }    
};

#endif