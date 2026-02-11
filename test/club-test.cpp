#include <chrono>
#include <gtest/gtest.h>

#include "club-state.hpp"

using namespace std::chrono_literals;
using namespace test_cpp;


class ClubStateTest : public ::testing::Test {
protected:
    ClubState club{3, 10, 8 * 60min, 17 * 60min};
};

TEST_F(ClubStateTest, TimeAndClientBasics) {
    EXPECT_TRUE(club.is_open(8 * 60min));
    EXPECT_FALSE(club.is_open(7 * 60min));
    EXPECT_FALSE(club.has_client("alice"));
    club.add_client("alice");
    EXPECT_TRUE(club.has_client("alice"));
}

TEST_F(ClubStateTest, SitGetUpAndRound) {
    club.add_client("bob");
    club.sit_client("bob", 1, 8 * 60min);
    EXPECT_TRUE(club.is_client_sit("bob"));
    club.get_up_from_table("bob", 9 * 60min + 1min);
    EXPECT_FALSE(club.is_client_sit("bob"));
}

TEST_F(ClubStateTest, TablesAndQueue) {
    club.add_client("a");
    club.add_client("b");
    club.add_client("c");
    club.sit_client("a", 1, 8 * 60min);
    club.sit_client("b", 2, 8 * 60min);
    club.sit_client("c", 3, 8 * 60min);
    EXPECT_FALSE(club.has_free_table());
    club.add_client("d");
    club.enqueue_client("d");
    EXPECT_FALSE(club.wait_queue_empty());
}

TEST_F(ClubStateTest, QueueOpsAndErrors) {
    club.add_client("x");
    club.enqueue_client("x");
    EXPECT_EQ(club.pop_waiting_client(), "x");
    EXPECT_THROW(club.sit_client("noone", 1, 8 * 60min), std::runtime_error);
    club.add_client("e");
    EXPECT_THROW(club.sit_client("e", 0, 8 * 60min), std::out_of_range);
}
