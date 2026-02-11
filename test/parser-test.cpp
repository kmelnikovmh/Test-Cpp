#include <sstream>
#include <gtest/gtest.h>

#include "parser.hpp"
#include "events.hpp"

using namespace test_cpp;


TEST(ParserTest, ParseClubValidConfig) {
    std::istringstream in("3\n08:00 22:00\n100\n");
    auto cfg = parse_club_config(in);
    EXPECT_EQ(cfg.table_count, 3);
    EXPECT_EQ(cfg.cost_per_hour, 100);
}

TEST(ParserTest, ParseEventsValidAndMultiple) {
    std::istringstream config_in("3\n08:00 22:00\n100\n");
    auto cfg = parse_club_config(config_in);
    std::istringstream in("08:00 1 alice\n08:10 2 alice 1\n08:20 3 alice\n");
    auto ev = parse_events(in, cfg);
    EXPECT_EQ(ev.size(), 3);
    EXPECT_TRUE(std::holds_alternative<EventClientArrived>(ev[0]));
}

TEST(ParserTest, ParseEventsErrorInvalidTable) {
    std::istringstream config_in("3\n08:00 22:00\n100\n");
    auto cfg = parse_club_config(config_in);
    std::istringstream in("08:00 2 alice 4\n");
    EXPECT_EXIT(parse_events(in, cfg), ::testing::ExitedWithCode(1), "");
}

TEST(ParserTest, ParseEventsErrorChronological) {
    std::istringstream config_in("3\n08:00 22:00\n100\n");
    auto cfg = parse_club_config(config_in);
    std::istringstream in("08:30 1 alice\n08:10 1 bob\n");
    EXPECT_EXIT(parse_events(in, cfg), ::testing::ExitedWithCode(1), "");
}

TEST(ParserTest, ParseEventsErrorTimeFormatAndName) {
    std::istringstream config_in("3\n08:00 22:00\n100\n");
    auto cfg = parse_club_config(config_in);
    std::istringstream in1("8:00 1 a\n");
    EXPECT_EXIT(parse_events(in1, cfg), ::testing::ExitedWithCode(1), "");
    std::istringstream in2("08:00 1 Alice\n");
    EXPECT_EXIT(parse_events(in2, cfg), ::testing::ExitedWithCode(1), "");
}

TEST(ParserTest, ComplexScenarioFullDay) {
    std::istringstream config_in("10\n00:00 23:59\n500\n");
    auto cfg = parse_club_config(config_in);
    std::istringstream in("00:00 1 a\n00:01 2 a_name-123 1\n23:59 1 xyz\n");
    auto ev = parse_events(in, cfg);
    EXPECT_EQ(ev.size(), 3);
}
