#include "run-day.hpp"

#include <variant>
#include <optional>
#include <string>
#include <vector>
#include <algorithm>


namespace test_cpp {

Events run_day(const EventsInput& input_events, ClubState& club) {
    Events result;
    for (auto& ev : input_events) {
        EventVariant ev_full = std::visit([](auto const& concrete) -> EventVariant {
            return concrete;
        }, ev);
        result.push_back(ev_full);

        std::optional<EventVariant> generated = std::visit([&club](auto const &concrete) {
            return concrete.apply_to_club(club);
        }, ev);

        if (generated.has_value()) {
            result.push_back(*generated);
        }
    }

    std::vector<std::string> remaining_clients = club.get_curr_clients();
    std::sort(remaining_clients.begin(), remaining_clients.end());
    for (const auto& name : remaining_clients) {
        if (club.is_client_sit(name)) {
            club.get_up_from_table(name, club.get_time_to_close());
        }
        club.remove_client(name);
        result.emplace_back(EventClientLeft(club.get_time_to_close(), static_cast<EventID>(11), name));
    }

    return result;
}

} // namespace test_cpp
