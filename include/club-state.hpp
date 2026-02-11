#ifndef TEST_CLUB_STATE_HPP
#define TEST_CLUB_STATE_HPP

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <chrono>


namespace test_cpp {

enum class ClientState {
    JustInClub,
    AtTable,
    Waiting
};

// Оставил некоторые классы публичными для простоты тестирования

struct ClientInfo {
    ClientState m_state = ClientState::JustInClub;
    int m_id_table = -1;
};

struct Table {
    bool m_is_free = true;
    std::chrono::minutes m_curr_start_busy = std::chrono::minutes::zero();
    std::chrono::minutes m_busy = std::chrono::minutes::zero();
    int m_salary = 0;
};

struct ClubInitConfig {
    int table_count;
    std::chrono::minutes open_time;
    std::chrono::minutes close_time;
    int cost_per_hour;
};

class ClubState {
private:
    std::vector<Table> m_tables;
    std::queue<std::string> m_wait_queue;
    std::unordered_map<std::string, ClientInfo> m_clients;

    int m_cost_table_per_hour;
    std::chrono::minutes m_open_time;
    std::chrono::minutes m_close_time;

public:
    ClubState(int table_count,
              int cost_per_hour,
              std::chrono::minutes open_time,
              std::chrono::minutes close_time) : m_tables(table_count),
                                                 m_cost_table_per_hour(table_count),
                                                 m_open_time(open_time),
                                                 m_close_time(close_time) {}

    ClubState(const ClubInitConfig &club_config) : m_tables(club_config.table_count),
                                                   m_cost_table_per_hour(club_config.cost_per_hour),
                                                   m_open_time(club_config.open_time),
                                                   m_close_time(club_config.close_time) {}

    // State
    bool is_open(std::chrono::minutes time) const;

    bool has_client(const std::string& name) const;

    bool is_client_sit(const std::string& name) const;

    bool has_free_table() const;

    bool is_free_table(int table_id) const;

    bool wait_queue_empty() const;

    bool wait_queue_full() const;

    void print_state() const;

    // Get
    std::chrono::minutes get_time_to_close() const;

    int get_id_table_from_client(const std::string& name) const;

    std::vector<std::string> get_curr_clients() const;

    // Set
    void add_client(const std::string& name);

    void sit_client(const std::string& name, int table_id, std::chrono::minutes time);

    void get_up_from_table(const std::string& name, std::chrono::minutes time);

    void enqueue_client(const std::string& name);

    std::string pop_waiting_client();

    void remove_client(const std::string& name);
};

// Думал еще написать классы для пользовательских исключений, но это похоже на оверкилл для тестового задания

} // namespace test_cpp

#endif
