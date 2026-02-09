#include "events.hpp"

#include <iostream>
#include <chrono>
#include <string>
#include <optional>
#include <variant>
#include <format>


namespace test_cpp {

// Корретность ввода это парсер
// Проверка на уровне клуба включает в себя только те случаи, которые нарушают правила cpp языка текущей реализации и ведут к UB: проверка обращения id, наличие клиента при обращении к его полям
// Вся бизнес логика проверки вынесена в apply_to_club, что дает максимальную гибкость в расширении процессов с минимальным переписыванием клуба. Например, добавление событий, которые могут происходить вне времени открытия клуба или с клиентами, которых еще нет в клубе
// В связи с этим, так как класс Клуба легко сломать вызвав любой сеттер вне бизнес логики, то данная возможная есть только у BaseEvent и его наследникво

// Проверка бизнес логики на уровне событий

BaseEvent::BaseEvent(std::chrono::minutes time_exec, EventID id) : m_time_exec(time_exec), m_id(id) {}

// EventClientArrived
EventClientArrived::EventClientArrived(std::chrono::minutes time_exec,
                                       EventID id,
                                       const std::string &client_name) : BaseEvent(time_exec, id),
                                                                         m_client_name(client_name) {}

std::optional<EventVariant> EventClientArrived::apply_to_club(ClubState &club) const {
    if (club.has_client(m_client_name)) {
        return EventError(m_time_exec, static_cast<EventID>(13), "YouShallNotPass");
    }
    if (!club.is_open(m_time_exec)) {
        return EventError(m_time_exec, static_cast<EventID>(13), "NotOpenYet");
    }
    club.add_client(m_client_name);
    return std::nullopt;
}

void EventClientArrived::print_fields() const {
    std::cout << std::format("{:%H:%M}", m_time_exec) << " " << static_cast<int>(m_id) << " " << m_client_name << "\n";
}

// EventClientSitDownAtTable
EventClientSitDownAtTable::EventClientSitDownAtTable(std::chrono::minutes time_exec,
                                                     EventID id,
                                                     const std::string &client_name,
                                                     int table_id) : BaseEvent(time_exec, id),
                                                                     m_client_name(client_name), 
                                                                     m_table_id(table_id) {}

// Нет по ТЗ NotOpenYet (учтено инвариантом клиенты уходят после закрытия и клиента нет раньше прихода)
std::optional<EventVariant> EventClientSitDownAtTable::apply_to_club(ClubState &club) const {
    if (!club.is_free_table(m_table_id)) {
        return EventError(m_time_exec, static_cast<EventID>(13), "PlaceIsBusy");
    }
    if (!club.has_client(m_client_name)) {
        return EventError(m_time_exec, static_cast<EventID>(13), "ClientUnknown");
    }
    if (club.is_client_sit(m_client_name)) {
        club.get_up_from_table(m_client_name, m_time_exec);
    }
    club.sit_client(m_client_name, m_table_id, m_time_exec);
    return std::nullopt;
}

void EventClientSitDownAtTable::print_fields() const {
    std::cout << std::format("{:%H:%M}", m_time_exec) << " " << static_cast<int>(m_id) << " "
              << m_client_name << " " << m_table_id << "\n";
}

// EventClientWait
EventClientWait::EventClientWait(std::chrono::minutes time_exec,
                                 EventID id,
                                 const std::string &client_name) : BaseEvent(time_exec, id),
                                                                   m_client_name(client_name) {}
// Нет по ТЗ ClientUnknown (упадет с ошибкой)
// А если он уже сидит? А если уже в очереди? Можно сломать все
std::optional<EventVariant> EventClientWait::apply_to_club(ClubState &club) const {
    if (club.has_free_table()) {
        return EventError(m_time_exec, static_cast<EventID>(13), "ICanWaitNoLonger!");
    }
    if (club.wait_queue_full()) {
        club.remove_client(m_client_name);
        return EventClientLeft(m_time_exec, static_cast<EventID>(11), m_client_name);
    }
    club.enqueue_client(m_client_name);
    return std::nullopt;
}

void EventClientWait::print_fields() const {
    std::cout << std::format("{:%H:%M}", m_time_exec) << " " << static_cast<int>(m_id) << " " << m_client_name << "\n";
}

// EventClientLeft
EventClientLeft::EventClientLeft(std::chrono::minutes time_exec,
                                 EventID id,
                                 const std::string &client_name) : BaseEvent(time_exec, id),
                                                                   m_client_name(client_name) {}

// А если он в очереди стоит в середине? Или все вставшие в очередь обязательно сядут?
std::optional<EventVariant> EventClientLeft::apply_to_club(ClubState &club) const {
    if (!club.has_client(m_client_name)) {
        return EventError(m_time_exec, static_cast<EventID>(13), "ClientUnknown");
    }
    int curr_table = 0;
    if (club.is_client_sit(m_client_name)) {
        curr_table = club.get_id_table_from_client(m_client_name);
        club.get_up_from_table(m_client_name, m_time_exec);
    }
    club.remove_client(m_client_name);
    if (curr_table && !club.wait_queue_empty()) {
        std::string next_client = club.pop_waiting_client();
        club.sit_client(next_client, curr_table, m_time_exec);
        return EventClientSitDownAtTable(m_time_exec, static_cast<EventID>(12), next_client, curr_table);
    }
    return std::nullopt;
}

void EventClientLeft::print_fields() const {
    std::cout << std::format("{:%H:%M}", m_time_exec) << " " << static_cast<int>(m_id) << " " << m_client_name << "\n";
}

// EventError
EventError::EventError(std::chrono::minutes time_exec,
                       EventID id,
                       const std::string &error) : BaseEvent(time_exec, id),
                                                   m_error(error) {}

void EventError::print_fields() const {
    std::cout << std::format("{:%H:%M}", m_time_exec) << " " << static_cast<int>(m_id) << " " << m_error << "\n";
}

} // namespace test_cpp
