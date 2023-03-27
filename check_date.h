#pragma once 
#include <iostream>

struct checking_time
{
    static bool check(const std::string& time) {
        int hours = 0;
        int minutes = 0;
        auto it = time.begin();
        size_t count_dig = 0;
        while(it != time.end() && *it != ':') {
            if (count_dig > 2) {
                return false;
            }
            if ('0' <= *it && *it <= '9') {
                hours = hours * 10 + (*it - '0');
            } else {
                return false;
            }
            ++count_dig;
            ++it;
        }
        if (count_dig > 2) {
            return false;
        }
        if (it == time.end()) {
            return false;
        }
        // ignore ':'
        ++it;
        count_dig = 0;
        while(it != time.end()) {
            if (count_dig > 2) {
                return false;
            }
            if ('0' <= *it && *it <= '9') {
                minutes = minutes * 10 + (*it - '0');
            } else {
                return false;
            }
            ++it;
            ++count_dig;
        }
        if (count_dig > 2) {
            return false;
        }

        return count_dig == 2 && hours < 24 && minutes < 60;
    }
};



