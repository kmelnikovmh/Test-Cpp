#include <chrono>
#include <gtest/gtest.h>

#include "club-state.hpp"
#include "events.hpp"
#include "run-day.hpp"

using namespace std::chrono_literals;
using namespace test_cpp;

class RunDayTest : public ::testing::Test {
protected:
    ClubState club{2, 100, 10 * 60min, 20 * 60min};
};

TEST_F(RunDayTest, EmptyDay) {
    EventsInput in;
    auto r = run_day(in, club);
    EXPECT_TRUE(r.empty());
}

TEST_F(RunDayTest, BasicFlowAndErrors) {
    EventsInput in{
        EventClientArrived(10 * 60min, EventID::ClientArrived, "a"),
        EventClientSitDownAtTable(10 * 60min + 1min, EventID::ClientSitDown, "a", 1),
        EventClientLeft(12 * 60min, EventID::ClientLeft, "a"),
        EventClientArrived(9 * 60min, EventID::ClientArrived, "b")
    };
    auto r = run_day(in, club);
    EXPECT_GE(r.size(), 3);
}

TEST_F(RunDayTest, BusyTableAndQueue) {
    EventsInput in{
        EventClientArrived(10 * 60min, EventID::ClientArrived, "a"),
        EventClientArrived(10 * 60min + 1min, EventID::ClientArrived, "b"),
        EventClientSitDownAtTable(10 * 60min + 2min, EventID::ClientSitDown, "a", 1),
        EventClientSitDownAtTable(10 * 60min + 3min, EventID::ClientSitDown, "b", 1),
        EventClientWait(11 * 60min, EventID::ClientWait, "c")
    };
    ClubState tc{2, 100, 10 * 60min, 20 * 60min};
    auto r = run_day(in, tc);
    bool has_error = false;
    for (auto &e: r) {
        if (std::holds_alternative<EventError>(e)) {
            has_error = true;
        }
    }
    EXPECT_TRUE(has_error);
}

TEST_F(RunDayTest, EndOfDaySystemLeft) {
    EventsInput in{ EventClientArrived(10 * 60min, EventID::ClientArrived, "x"),
                    EventClientSitDownAtTable(10 * 60min + 1min, EventID::ClientSitDown, "x",1)};
    ClubState tc{2, 100, 10 * 60min, 20 * 60min};
    auto r = run_day(in, tc);
    bool found = false;
    for (auto &e: r) {
        if (std::holds_alternative<EventClientLeft>(e)) {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(RunDayTest, ErrorsDoNotCrash) {
    EventsInput in{ EventClientArrived(10 * 60min, EventID::ClientArrived, "a"),
                    EventClientArrived(10 * 60min + 1min, EventID::ClientArrived, "a") };
    ClubState tc{2, 100, 10 * 60min, 20 * 60min};
    auto r = run_day(in, tc);
    EXPECT_GE(r.size(), 1);
}
