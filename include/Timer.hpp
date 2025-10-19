#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>

class Timer {
    //using time = chrono::steady_clock;
    private :
        std::chrono::steady_clock::time_point alpha, end;
        std::chrono::duration<double> elapsed;
        uint64_t cycle;

        void update () {
            end = std::chrono::steady_clock::now ();
            elapsed = end - alpha;
        }
    public :
        Timer() {
            alpha = std::chrono::steady_clock::now ();
            cycle = 0;
        }
        void start () { alpha = std::chrono::steady_clock::now ();}
        void stop () { update();}
        void get_duration () {
            double duration = elapsed.count();
            const std::vector<std::pair<int,std::string>> base {{31536000,"year"},{86400,"day"}, {3600,"hour"},{60,"mn"}, {1,"s"}};
            int count = 0;
            std::vector<int> period;
            std::ostringstream os;

            for (auto it : base) {
                period.push_back (duration / it.first);

                if (period.back() != 0)
                    count++;

                duration -= (period.back() * it.first);
            }
            for (int i = 0; i < 5; ++i) {
                if (period[i] != 0) {
                    count--;
                    os << period[i] << " " << base[i].second;
                    // if (period[i] > 1) os << "s";
                    if (count > 1) os << ", ";
                    if (count == 1) os << " ";
                }
            }
            if (duration > 0.0) os << " " << int(duration * 60 * 1e3) / 1e3 << " ms";

            std::cout << "\nDuration " << os.str() << std::endl;
        }
        bool running (double total) {
            update();
            cycle++;
            if (elapsed.count() < total) return true;
            std::cout << "cycles :: " ;

            std::string num = std::to_string (cycle);
            int size = num.size(), index = 0;

            while (size-->0) {
                std::cout << num[index++];
                if (size % 3 == 0) std::cout << ' ';
            }
            std::cout << std::endl;
            return false;
        }
};
