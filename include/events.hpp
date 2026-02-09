#ifndef _TEST_EVENT_
#define _TEST_EVENT_

#include <chrono>
#include <string>
#include <optional>
#include <variant>
#include <vector>

#include "club-state.hpp"


namespace test_cpp {

// Можно было бы сделать через наследование и виртуальные фнукции, но в учебных целях захотелось сделать std::variant,
// который должен работать побыстрее виртуальных таблиц

enum class EventID : int {
    ClientArrived = 1,
    ClientSitDown = 2,
    ClientWait = 3,
    ClientLeft = 4,
    SystemClientLeft = 11,
    SystemClientSitDown = 12,
    Error = 13
};

class EventClientArrived;
class EventClientSitDownAtTable;
class EventClientWait;
class EventClientLeft;
class EventError;

using EventVariantInput = std::variant<EventClientArrived,
                                       EventClientSitDownAtTable,
                                       EventClientWait,
                                       EventClientLeft>;

using EventsInput = std::vector<EventVariantInput>;

using EventVariant = std::variant<EventClientArrived,
                                  EventClientSitDownAtTable,
                                  EventClientWait,
                                  EventClientLeft,
                                  EventError>;

using Events = std::vector<EventVariant>;

class BaseEvent {
protected:
    std::chrono::minutes m_time_exec;
    EventID m_id;
    BaseEvent(std::chrono::minutes time_exec, EventID id);
};

class EventClientArrived : BaseEvent {
private:
    std::string m_client_name;
public:
    EventClientArrived(std::chrono::minutes time_exec, EventID id, const std::string &client_name);
    std::optional<EventVariant> apply_to_club(ClubState &club) const;
    void print_fields() const;
};

class EventClientSitDownAtTable : BaseEvent {
private:
    std::string m_client_name;
    int m_table_id;
public:
    EventClientSitDownAtTable(std::chrono::minutes time_exec, EventID id, const std::string &client_name, int table_id);
    std::optional<EventVariant> apply_to_club(ClubState &club) const;
    void print_fields() const;
};

class EventClientWait : BaseEvent {
private:
    std::string m_client_name;
public:
    EventClientWait(std::chrono::minutes time_exec, EventID id, const std::string &client_name);
    std::optional<EventVariant> apply_to_club(ClubState &club) const;
    void print_fields() const;
};

class EventClientLeft : BaseEvent {
private:
    std::string m_client_name;
public:
    EventClientLeft(std::chrono::minutes time_exec, EventID id, const std::string &client_name);
    std::optional<EventVariant> apply_to_club(ClubState &club) const;
    void print_fields() const;
};

class EventError : BaseEvent {
private:
    std::string m_error;
public:
    EventError(std::chrono::minutes time_exec, EventID id, const std::string &error);
    void print_fields() const;
};

} // namespace test_cpp

#endif
