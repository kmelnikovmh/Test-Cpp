#include "parser.hpp"
#include "events.hpp"
#include "club-state.hpp"
#include "run-day.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <format>


// Вообще выводить бы ошибки в std::cerr...

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Invalid count argc\n";
        return 1;
    }
    std::filesystem::path file_path = std::filesystem::path(argv[1]);;
    std::ifstream file_cin(file_path);
    if (!file_cin) {
        std::cout << "File " << file_path << " does not exist!\n";
        return 1;
    }

    auto [table_count, open_time, close_time, cost_per_hour] = test_cpp::parse_club_config(file_cin);
    test_cpp::ClubState club(table_count, cost_per_hour, open_time, close_time);

    auto config = test_cpp::ClubInitConfig{table_count, open_time, close_time, cost_per_hour};
    test_cpp::EventsInput input_events = test_cpp::parse_events(file_cin, config);

    test_cpp::Events all_events = test_cpp::run_day(input_events, club);

    std::cout << std::format("{:%H:%M}", open_time) << "\n";
    for (auto& ev : all_events) {
        std::visit([&](auto& e){ e.print_fields(); }, ev);
    }
    std::cout << std::format("{:%H:%M}", close_time) << "\n";
    club.print_state();
}
