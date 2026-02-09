#include "parser.hpp"

#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <string>
#include <chrono>


namespace test_cpp {

std::chrono::minutes parse_time(const std::string& s, const std::string& line) {
    if (s.size() != 5 || s[2] != ':') {
        std::cout << line << "\n"; 
        std::exit(1);
    }
    if (!std::isdigit(s[0]) || !std::isdigit(s[1]) || 
        !std::isdigit(s[3]) || !std::isdigit(s[4])) {
        std::cout << line << "\n"; 
        std::exit(1);
    }
    int h = std::stoi(s.substr(0, 2));
    int m = std::stoi(s.substr(3, 2));
    if (h < 0 || 23 < h || m < 0 || 59 < m) {
        std::cout << line << "\n"; 
        std::exit(1);
    }
    return std::chrono::hours(h) + std::chrono::minutes(m);
}

bool valid_client_name(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    for (char c : name) {
        if (!(std::islower(c) || std::isdigit(c) || c == '_' || c == '-')) {
            return false;
        }
    }
    return true;
}

bool has_extra_params(std::istringstream& iss) {
    std::string extra;
    return static_cast<bool>(iss >> extra);
}

ClubInitConfig parse_club_config(std::istream& in) {
    std::string line;
    if (!std::getline(in, line)) { 
        std::cout << "\n"; 
        std::exit(1); 
    }

    std::istringstream iss1(line);
    int tables; 
    if (!(iss1 >> tables) || tables <= 0 || has_extra_params(iss1)) { 
        std::cout << line << "\n"; 
        std::exit(1); 
    }

    if (!std::getline(in, line)) { 
        std::cout << "\n"; 
        std::exit(1); 
    }
    std::istringstream iss2(line);
    std::string open, close;
    if (!(iss2 >> open >> close) || has_extra_params(iss2)) { 
        std::cout << line << "\n"; 
        std::exit(1); 
    }
    auto open_time = parse_time(open, line);
    auto close_time = parse_time(close, line);

    if (!std::getline(in, line)) { 
        std::cout << "\n"; 
        std::exit(1); 
    }
    std::istringstream iss3(line);
    int cost;
    if (!(iss3 >> cost) || cost <= 0 || has_extra_params(iss3)) { 
        std::cout << line << "\n"; 
        std::exit(1); 
    }

    return {tables, open_time, close_time, cost};
}

EventsInput parse_events(std::istream& in, const ClubInitConfig& config) {
    EventsInput events;
    std::string line;
    std::chrono::minutes prev_time = std::chrono::minutes(-1);
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        std::istringstream iss(line);
        std::string time_str, name;
        int id;

        if (!(iss >> time_str >> id)) { 
            std::cout << line << "\n"; 
            std::exit(1); 
        }
        auto time = parse_time(time_str, line);
        if (prev_time != std::chrono::minutes(-1) && time < prev_time) {
            std::cout << line << "\n"; 
            std::exit(1);
        }
        prev_time = time;

        if (id == 1) {
            if (!(iss >> name) || !valid_client_name(name) || has_extra_params(iss)) { 
                std::cout << line << "\n"; 
                std::exit(1); 
            }
            events.emplace_back(EventClientArrived{time, static_cast<EventID>(1), name});
        } else if (id == 2) {
            int table;
            if (!(iss >> name >> table) || !valid_client_name(name) || table <= 0 || 
                table > config.table_count || has_extra_params(iss)) { 
                std::cout << line << "\n"; 
                std::exit(1); 
            }
            events.emplace_back(EventClientSitDownAtTable{time, static_cast<EventID>(2), name, table});
        } else if (id == 3) {
            if (!(iss >> name) || !valid_client_name(name) || has_extra_params(iss)) { 
                std::cout << line << "\n"; 
                std::exit(1); 
            }
            events.emplace_back(EventClientWait{time, static_cast<EventID>(3), name});
        } else if (id == 4) {
            if (!(iss >> name) || !valid_client_name(name) || has_extra_params(iss)) { 
                std::cout << line << "\n"; 
                std::exit(1); 
            }
            events.emplace_back(EventClientLeft{time, static_cast<EventID>(4), name});
        } else {
            std::cout << line << "\n"; 
            std::exit(1);
        }
    }
    return events;
}

} // namespace test_cpp
