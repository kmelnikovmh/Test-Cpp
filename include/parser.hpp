#ifndef TEST_PARSER_HPP
#define TEST_PARSER_HPP

#include <string>
#include <chrono>
#include <istream>

#include "events.hpp"
#include "club-state.hpp"


namespace test_cpp {

ClubInitConfig parse_club_config(std::istream& in);

EventsInput parse_events(std::istream& in, const ClubInitConfig& config);

} // namespace test_cpp

#endif
