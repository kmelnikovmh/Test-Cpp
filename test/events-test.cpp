#include <chrono>
#include <optional>
#include <gtest/gtest.h>

#include "club-state.hpp"
#include "events.hpp"

using namespace std::chrono_literals;
using namespace test_cpp;


class EventsTest : public ::testing::Test {
protected:
    ClubState club{2, 100, 10 * 60min, 20 * 60min};
};

TEST_F(EventsTest, ArriveAndSit) {
    EventClientArrived a(12 * 60min, EventID::ClientArrived, "alice");
    EXPECT_FALSE(a.apply_to_club(club).has_value());
    EventClientSitDownAtTable s(12 * 60min + 1min, EventID::ClientSitDown, "alice", 1);
    EXPECT_FALSE(s.apply_to_club(club).has_value());
    EXPECT_TRUE(club.is_client_sit("alice"));
}

TEST_F(EventsTest, ArriveErrors) {
    club.add_client("x");
    EventClientArrived e(9 * 60min, EventID::ClientArrived, "y");
    EXPECT_TRUE(e.apply_to_club(club).has_value());
}

TEST_F(EventsTest, SitErrorsAndChange) {
    club.add_client("a");
    club.sit_client("a", 1, 10 * 60min);
    EventClientSitDownAtTable e1(11 * 60min, EventID::ClientSitDown, "b", 1);
    EXPECT_TRUE(e1.apply_to_club(club).has_value());
    club.add_client("b");
    EventClientSitDownAtTable e2(11 * 60min, EventID::ClientSitDown, "b", 1);
    EXPECT_TRUE(e2.apply_to_club(club).has_value());
}

TEST_F(EventsTest, WaitAndOverflow) {
    club.add_client("a");
    club.add_client("b");
    club.sit_client("a", 1, 10 * 60min);
    club.sit_client("b", 2, 10 * 60min);
    club.add_client("c");
    EventClientWait w(11 * 60min, EventID::ClientWait, "c");
    EXPECT_FALSE(w.apply_to_club(club).has_value());
}

TEST_F(EventsTest, LeftAndSystemSit) {
    club.add_client("a");
    club.add_client("b");
    club.add_client("c");
    club.sit_client("a", 1, 10 * 60min);
    club.sit_client("b", 2, 10 * 60min);
    club.enqueue_client("c");
    EventClientLeft left(12 * 60min, EventID::ClientLeft, "a");
    auto res = left.apply_to_club(club);
    EXPECT_TRUE(res.has_value());
    EXPECT_TRUE(std::holds_alternative<EventClientSitDownAtTable>(*res));
}
