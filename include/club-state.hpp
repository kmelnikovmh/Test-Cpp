#ifndef _TEST_CLUB_STATE_
#define _TEST_CLUB_STATE_

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
    ClientInfo const *m_curr_client = nullptr;
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
    std::chrono::minutes m_time_to_open;
    std::chrono::minutes m_time_to_close;

public:
    ClubState(int count_tables,
              int cost_table_per_hour,
              std::chrono::minutes time_to_open,
              std::chrono::minutes time_to_close) : m_tables(count_tables),
                                                    m_cost_table_per_hour(cost_table_per_hour),
                                                    m_time_to_open(time_to_open),
                                                    m_time_to_close(time_to_close) {}

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

} // namespace test_cpp

#endif
