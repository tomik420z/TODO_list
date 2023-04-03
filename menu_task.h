#pragma once
#ifndef MENU_TASK_H
#define MENU_TASK_H
#include "data_task.h"
#include <windows.h>

class menu_task {
    static void ignore_spaces() {
        using std::cin;
        while (cin.peek() == ' ') {
            cin.ignore();    
        }
    }

    static void print_menu() {
        std::cout << "MENU" << std::endl;
        std::cout << "--------------------------------" << std::endl;
        std::cout << "1. ADD NEW TASK" << std::endl;
        std::cout << "2. REMOVE TASK" << std::endl;
        std::cout << "3. RESCHEDULE THE EVENT" << std::endl;
        std::cout << "4. SHOW TASKS FOR THE GIVEN DAY" << std::endl;
        std::cout << "5. SHOW TASKS FOR TODAY" << std::endl;
        std::cout << "6. SHOW DATES ON WHICH TASKS ARE ASSIGNED" << std::endl;
        std::cout << "7. EDITING A TASK" << std::endl;
        std::cout << "8. SAVE AND EXIT" << std::endl;
    }

    static int input_integer() {
        using std::cin;
        int result = 0; 
        ignore_spaces();

        while('0' <= cin.peek() && cin.peek() <= '9') {
           result = result * 10 + (cin.get() - '0'); 
        }

        ignore_spaces();

        if (cin.peek() != '\n') {
            while(cin.peek() != '\n') {
                cin.ignore();
            }
            cin.ignore();
            
            throw std::string("you need to enter a number from 1 to 7");
        }  else {
            cin.ignore();
        }

        return result;
    }

    static std::string input_date() {
        using std::cin;
        
        std::string new_date;
        ignore_spaces();
        
        while (cin.peek() != ' ' && cin.peek() != '\n') {
            new_date += cin.get();
        }
        
        ignore_spaces();
        if (cin.peek() != '\n') {
            while(cin.peek() != '\n') {
                cin.ignore();
            }
            cin.ignore();
            throw exception_data_task::incorrect_format_date();

        }  else {
            cin.ignore();
        }

        if (new_date == "today") {
            new_date = calendar::to_sql_string(calendar::day_clock::local_day());
        } else if (new_date == "tomorrow") {
            new_date = calendar::to_sql_string(calendar::day_clock::local_day() + calendar::days(1));
        } else if (new_date == "yesterday") {
            new_date = calendar::to_sql_string(calendar::day_clock::local_day() - calendar::days(1));
        } else {
            try {
                calendar::date d = calendar::from_string(new_date); 
            } catch (...) {
                throw exception_data_task::incorrect_format_date();
            } 
        }

        return new_date;
    }

    static std::string input_task() {
        std::string task;
        std::getline(std::cin, task, '\n');
        return task;
    }

    static boost::posix_time::time_duration round_time(bool& flag_error) {
        using namespace boost;
        posix_time::ptime loc_time(boost::posix_time::second_clock::local_time());
        posix_time::time_duration d = loc_time.time_of_day();
        auto t_minutes = d.minutes();
        auto t_hours = d.hours();
        if (t_minutes < 15) {
            t_minutes = 0; 
        } else if (t_minutes >= 15 && t_minutes < 30) {
            t_minutes = 30;
        } else if (t_minutes >= 30 && t_minutes < 45) {
            t_minutes = 30;
        } else {
            t_minutes = 0;
            if (t_hours < 23) {
                t_hours += 1; 
            } else {
                flag_error = true;
            }
        }
        return posix_time::time_duration(t_hours, t_minutes, 0);
    }

    static std::string input_start_time() {
        using std::cin;
        std::string new_start;
        ignore_spaces();
        while('0' <= cin.peek() && cin.peek() <= '9' || cin.peek() == ':') {
            new_start += cin.get();
        }
        if (new_start.size() == 4) {
            new_start.insert(new_start.begin(), '0');
        }
        ignore_spaces();
        if (cin.peek() == '\n') {
            cin.ignore();
        } else {
            while(cin.peek() != '\n') {
                cin.ignore();
            }
            cin.ignore();
            throw exception_data_task::incorrect_format_time();
        }

        if (new_start.empty()) {
            bool flag = false;
            new_start = boost::posix_time::to_simple_string(round_time(flag));
            new_start.pop_back();
            new_start.pop_back();
            new_start.pop_back();
        }

        return new_start;
    }

    static std::string input_end_time() {
        using std::cin;
        using boost::posix_time::time_duration;
        
        std::string new_end;
        ignore_spaces();
        
        while('0' <= cin.peek() && cin.peek() <= '9' || cin.peek() == ':') {
            new_end += cin.get();
        }
        if (new_end.size() == 4) {
            new_end.insert(new_end.begin(), '0');
        }
        
        ignore_spaces();
        
        if (cin.peek() == '\n') {
            cin.ignore();
        } else {
            while(cin.peek() != '\n') {
                cin.ignore();
            }
            cin.ignore();
            throw exception_data_task::incorrect_format_time();
        }

        if (new_end.empty()) {
            bool flag = false;
            new_end = boost::posix_time::to_simple_string(round_time(flag) + boost::posix_time::hours(1));
            new_end.pop_back();
            new_end.pop_back();
            new_end.pop_back();
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
                throw std::string("you need to enter a number from 1 to 8");
            }
        }
        return num;

    } 

    static char input_select() {
        using std::cin;
        char select;
        bool flag_correct = false;
        while(!flag_correct) {
            while (cin.peek() == ' ' || cin.peek() == '\n')  {
                cin.ignore();
            } 
            if(cin.peek() == 'y' || cin.peek() == 'n') {
                select = cin.get();
            } else {
                while(cin.peek() != '\n') {
                    cin.ignore();
                }
                cin.ignore();
                continue;
            } 

            ignore_spaces();
            if (cin.peek() == '\n') {
                flag_correct = true;
                cin.ignore();
            } else {
                while(cin.peek() != '\n') {
                    cin.ignore();
                }
                cin.ignore();
                continue;
            }
        }
        return select;

    }

    static std::vector<std::string> input_comment() {
        using std::cin;
        std::vector<std::string> comments;
        size_t i = 1;
        std::string tmp_str;
        do {
            std::cout << i << ".";
            ignore_spaces();
            while(cin.peek() != '\n') {
                tmp_str += cin.get();
            }
            cin.get();

            while(!tmp_str.empty() && tmp_str.back() == ' '){
                tmp_str.pop_back();
            }

            if (!tmp_str.empty()) {
                comments.emplace_back(std::move(tmp_str));
            } else {
                break;
            }
            ++i;
        } while(true);
        return comments;
    }


public:

    static void exec(const char* f_name) {
        
        HANDLE h;
        h = GetStdHandle(STD_OUTPUT_HANDLE); 
        data_task data(f_name);
        if (data.get_flag_saved() == true) {
            std::cout << "The program was not terminated correctly last time, do you want to make any unsaved changes?(y/n)";
            char select = input_select();
            if (select == 'y') {
                data.read_tmp_files();
                data.write_to_json();
            } else {
                data.clear_tmp_dir();
            }
        }

        int select_index = 0;
        system("cls");
        print_menu();
        while(select_index != -1) {
            try {
            
                std::cout << "select a number from 1 to 6" << std::endl;
                select_index = input_integer();
                system("cls");
                print_menu();
                switch(select_index) {
                case 1: 
                    {
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
                        
                        std::cout << "select a number from 1 to 3" << std::endl;
                        size_t select_priority = input_integer();
                        std::vector<std::string> comments;
                        std::cout << "whether you want to add comments to the task? (y/n)" << std::endl;
                        char ch = input_select();
                        if (ch == 'y') {
                            std::cout << "enter a list of comments to continue, press key enter" << std::endl;
                            comments = input_comment();
                        } 
                        data.add_new_task(std::move(new_date), std::move(new_task), std::move(new_start), std::move(new_end), select_priority, std::move(comments));
                        
                        system("cls");
                        print_menu();
                        
                        SetConsoleTextAttribute(h, 02);
                        std::cout << "task added successfully!" << std::endl;
                        SetConsoleTextAttribute(h, 07);

                        break;
                    }
                case 2:
                    {
                        std::cout << "enter date:"<< std::endl;
                        std::string erase_date = input_date();
                        if (auto it_find = data.choose_date(erase_date); it_find != data.end()) {
                            auto& ref_set = it_find->second;
                            data.print_data_set(ref_set);
                            std::cout << ref_set.size() + 1 << ". ya peredumal udalat'" << std::endl;
                            std::cout << "select number from " << 1 << " to " << ref_set.size() + 1 << ":" << std::endl;
                            size_t select_index = input_integer();
                            if (select_index == ref_set.size() + 1) {
                                system("cls");
                                print_menu();
                                break;
                            }
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
                        std::cout << "enter date:"<< std::endl;
                        std::string erase_date = input_date();
                        if (auto it_find = data.choose_date(erase_date); it_find != data.end()) {
                            auto& ref_set = it_find->second;
                            data.print_data_set(ref_set);
                            std::cout << ref_set.size() + 1 << ". ya peredumal perenosit'" << std::endl;
                            std::cout << "select number from " << 1 << " to " << ref_set.size() + 1 << ":" << std::endl;
                            size_t select_index = input_integer();   
                            if (select_index == ref_set.size() + 1) {
                                system("cls");
                                print_menu();
                                break;
                            }
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
                        std::cout << "enter date:" << std::endl;
                        std::string date = input_date();
                        data.show_tasks_for_the_given_day(date);
                        break;
                    }
                case 5:
                    {
                        data.show_tasks_for_the_given_day(calendar::to_sql_string(calendar::day_clock::local_day()));
                        break;
                    }
                case 6:
                    {
                        
                        std::cout << "you want to also show the tasks along with the dates? (y/n)" << std::endl;
                        char ch = input_select();
                        system("cls");
                        print_menu();
                        std::cout << "input date start: " << std::endl;
                        std::string date_start = input_date();
                        std::cout << "input date finish: " << std::endl;
                        std::string date_last = input_date();
                        if (date_start > date_last) {
                            throw std::string("incorrect date interval");
                        }
                        if (ch == 'y')  {
                            data.show_dates_with_tasks(date_start, date_last); 
                        } else {
                            data.show_date(date_start, date_last);
                        }
                        break;
                    }
                case 7: 
                {                
                    break;
                }
                case 8:
                {
                    select_index = -1;
                    break;
                }
                default:
                    {                
                        throw std::string("you need to enter a number from 1 to 8");
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