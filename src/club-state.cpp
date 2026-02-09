#include "club-state.hpp"

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <chrono>


namespace test_cpp {

constexpr int ROUNDING_BIAS = 59;

// State
bool ClubState::is_open(std::chrono::minutes time) const {
    return m_time_to_open <= time && time < m_time_to_close;
}

bool ClubState::has_client(const std::string& name) const {
    return m_clients.find(name) != m_clients.end();
}

bool ClubState::is_client_sit(const std::string& name) const {
    auto it_client = m_clients.find(name);
    if (it_client == m_clients.end()) {
        throw std::runtime_error("Client " + name + " not found");
    }
    return it_client->second.m_state == ClientState::AtTable;
}

bool ClubState::has_free_table() const {
    return std::any_of(m_tables.begin(), 
                       m_tables.end(),
                       [](const Table& t){ return t.m_is_free == true; });
}

bool ClubState::is_free_table(int table_id) const {
    int idx = table_id - 1;
    if (idx < 0 || static_cast<int>(m_tables.size()) <= idx) {
        throw std::runtime_error("Incorrect table id: " + std::to_string(table_id));
    }
    return m_tables[idx].m_is_free == true;
}

bool ClubState::wait_queue_empty() const {
    return m_wait_queue.empty();
}

bool ClubState::wait_queue_full() const {
    return m_tables.size() <= m_wait_queue.size();
}

void ClubState::print_state() const {
    for (int i = 1; i <= static_cast<int>(m_tables.size()); ++i) {
        const Table& table = m_tables[i - 1];
        int h = table.m_busy.count() / 60;
        int m = table.m_busy.count() % 60;
        std::cout << i << " " << table.m_salary << " "
                  << std::setw(2) << std::setfill('0') << h
                  << ":" << std::setw(2) << std::setfill('0') << m
                  << "\n";
    }
}

// Get
std::chrono::minutes ClubState::get_time_to_close() const {
    return m_time_to_close;
}

int ClubState::get_id_table_from_client(const std::string& name) const {
    auto it_client = m_clients.find(name);
    if (it_client == m_clients.end()) {
        throw std::runtime_error("Client " + name + " not found");
    }
    return it_client->second.m_id_table;
}

std::vector<std::string> ClubState::get_curr_clients() const {
    std::vector<std::string> remaining_clients;
    for (auto& [name, client] : m_clients) {
        remaining_clients.push_back(name);
    }

    return remaining_clients;
}

// Set
void ClubState::add_client(const std::string& name) {
    m_clients[name] = ClientInfo{};
}

void ClubState::sit_client(const std::string& name, int table_id, std::chrono::minutes time) {
    int idx = table_id - 1;
    if (!has_client(name)) {
        throw std::runtime_error("Client " + name + " not found");
    }
    if (idx < 0 || static_cast<int>(m_tables.size()) <= idx) {
        throw std::out_of_range("Incorrect table id: " + std::to_string(table_id));
    }
    Table& table = m_tables[idx];
    table.m_is_free = false;
    table.m_curr_start_busy = time;

    ClientInfo& client_info = m_clients[name];
    client_info.m_state = ClientState::AtTable;
    client_info.m_id_table = table_id;
}

void ClubState::get_up_from_table(const std::string& name, std::chrono::minutes time) {
    auto it_client = m_clients.find(name);
    if (it_client == m_clients.end()) {
        throw std::runtime_error("Client " + name + " not found");
    }

    ClientInfo& client_info = it_client->second;
    if (client_info.m_state != ClientState::AtTable) {
        return;
    }
    int table_idx = client_info.m_id_table - 1;
    Table& table = m_tables[table_idx];

    auto duration = time - table.m_curr_start_busy;
    table.m_busy += duration;
    int hours = (duration.count() + ROUNDING_BIAS) / 60;
    table.m_salary += hours * m_cost_table_per_hour;

    table.m_is_free = true;
    table.m_curr_start_busy = std::chrono::minutes::zero();

    client_info.m_state = ClientState::JustInClub;
    client_info.m_id_table = -1;
}

void ClubState::enqueue_client(const std::string& name) {
    auto it_client = m_clients.find(name);
    if (it_client == m_clients.end()) {
        throw std::runtime_error("Client " + name + " not found");
    }
    ClientInfo& client_info = it_client->second;
    client_info.m_state = ClientState::Waiting;
    client_info.m_id_table = -1;
    m_wait_queue.push(name);
}

std::string ClubState::pop_waiting_client() {
    if (m_wait_queue.empty()) {
        throw std::runtime_error("Queue empty, can't call pop_waiting_client");
    }
    std::string name = m_wait_queue.front();
    m_wait_queue.pop();
    return name;
}

void ClubState::remove_client(const std::string& name) {
    auto it_client = m_clients.find(name);
    if (it_client == m_clients.end()) {
        throw std::runtime_error("Client " + name + " not found");
    }
    m_clients.erase(it_client);
}

} // namespace test_cpp
