
# Описание API и классов

Ниже структурированное и краткое описание основных типов, классов и их публичных методов в проекте.

**Parser**

- **`parse_club_config(std::istream&) -> ClubInitConfig`**: парсит три строки конфигурации клуба: число столов, время открытия/закрытия (формат HH:MM), стоимость часа; валидирует формат и диапазоны, в случае ошибки завершает программу.
- **`parse_events(std::istream&, const ClubInitConfig&) -> EventsInput`**: читает журнал событий построчно, проверяет корректность времен (возрастающий порядок), идентификаторов событий и полей; формирует `EventsInput` (входной набор событий). Валидация тут синтаксическая и базовая семантическая (номера столов в диапазоне), сложная бизнес-логика не выполняется.

**ClubState**

Представляет текущее состояние клуба: столы, клиенты и очередь. Конструктор принимает количество столов, стоимость часа и время открытия/закрытия.

- Конструктор: `ClubState(int count_tables, int cost_table_per_hour, std::chrono::minutes time_to_open, std::chrono::minutes time_to_close)`

Состояние / проверки:
- `bool is_open(std::chrono::minutes time) const` — открыт ли клуб в момент `time`.
- `bool has_client(const std::string& name) const` — есть ли клиент с именем `name` в состоянии.
- `bool is_client_sit(const std::string& name) const` — сидит ли клиент за столом (выбрасывает `runtime_error`, если клиента нет).
- `bool has_free_table() const` — есть ли свободный стол.
- `bool is_free_table(int table_id) const` — свободен ли конкретный стол (браузит `out_of_range` при некорректном id).
- `bool wait_queue_empty() const` — пуста ли очередь ожидания.
- `bool wait_queue_full() const` — заполнена ли очередь (максимум = число столов).
- `void print_state() const` — печать итоговой информации по столам (`salary` и накопленное время).

Геттеры:
- `std::chrono::minutes get_time_to_close() const`
- `int get_id_table_from_client(const std::string& name) const` — возвращает id стола клиента (ошибка при отсутствии клиента)
- `std::vector<std::string> get_curr_clients() const` — список имен текущих клиентов в состоянии

Модификаторы (изменяют состояние):
- `void add_client(const std::string& name)` — добавляет клиента в состояние (по умолчанию `JustInClub`).
- `void sit_client(const std::string& name, int table_id, std::chrono::minutes time)` — садит клиента за стол: помечает стол занят, устанавливает время начала, обновляет `ClientInfo`.
- `void get_up_from_table(const std::string& name, std::chrono::minutes time)` — клиент встает со стола: считает длительность, округляет до часов (см. ROUNDING_BIAS = 59), начисляет `m_salary`, освобождает стол и переводит клиента в `JustInClub`.
- `void enqueue_client(const std::string& name)` — переводит клиента в состояние ожидания и ставит в очередь.
- `std::string pop_waiting_client()` — извлекает имя следующего в очереди (ошибка, если очередь пуста).
- `void remove_client(const std::string& name)` — удаляет клиента из состояния (ошибка, если клиента нет).

Типы данных внутри `club-state.hpp`:
- `enum class ClientState { JustInClub, AtTable, Waiting }`
- `struct ClientInfo { ClientState m_state; int m_id_table; }`
- `struct Table { bool m_is_free; std::chrono::minutes m_curr_start_busy; std::chrono::minutes m_busy; int m_salary; }`
- `struct ClubInitConfig { int table_count; std::chrono::minutes open_time; std::chrono::minutes close_time; int cost_per_hour; }`

Поведение и исключения:
- Методы, которые ожидают существование клиента/стола, кидают `std::runtime_error` или `std::out_of_range` при неверных аргументах. Учет времени и подсчет оплаты происходит в `get_up_from_table`.

**Events / события**

События реализованы value-based через `std::variant` (вместо иерархии с виртуальными методами).

- `enum class EventID` — идентификаторы событий (ClientArrived, ClientSitDown и т.д.).
- `class BaseEvent` — содержит время исполнения `m_time_exec` и `EventID m_id` (защищенно).

Классы конкретных событий (все имеют конструкторы и три основных метода):
- `EventClientArrived(std::chrono::minutes time, EventID id, const std::string& name)`
- `EventClientSitDownAtTable(std::chrono::minutes time, EventID id, const std::string& name, int table_id)`
- `EventClientWait(std::chrono::minutes time, EventID id, const std::string& name)`
- `EventClientLeft(std::chrono::minutes time, EventID id, const std::string& name)`
- `EventError(std::chrono::minutes time, EventID id, const std::string& error)`

Общий контракт для событий:
- `std::optional<EventVariant> apply_to_club(ClubState &club) const` — применяет событие к `ClubState`. Возвращает опционально сгенерированное системное событие (например, пересадка из очереди) или `EventError` при нарушениях.
- `void print_fields() const` — печатает событие в stdout в исходном формате.

Ключевые ситуации (реализация в `events.cpp`):
- `EventClientArrived` — проверяет, что клиент еще не существует и клуб открыт; иначе возвращает `EventError`.
- `EventClientSitDownAtTable` — проверяет, что стол свободен и клиент известен; если клиент уже сидит — сначала вызывает `get_up_from_table`, затем садит за новый стол.
- `EventClientWait` — если есть свободный стол — возвращает `EventError`; если очередь полна — удаляет клиента и генерирует системное `ClientLeft`.
- `EventClientLeft` — если клиент сидел — вызывает `get_up_from_table`; удаляет клиента; если оказался свободный стол и есть очередь — автоматически пересаживает следующего и генерирует `SystemClientSitDown`.
- `EventError` — только для логирования, не меняет состояние клуба.

**Run-day**

- `Events run_day(const EventsInput& input_events, ClubState& club)` — основной цикл применения входных событий в порядке их поступления:
	- Каждый входной event из `EventsInput` добавляется в результирующий журнал `Events`.
	- Вызывается `apply_to_club` для изменения `ClubState`; если возвращается доп. событие — оно также добавляется в журнал.
	- После применения всех входных событий обрабатываются оставшиеся клиенты: если кто-то сидит — вызывается `get_up_from_table` с временем закрытия клуба; затем все клиенты удаляются и в журнал добавляются системные `ClientLeft` (EventID 11).

Примечание: детали округления времени (вплоть до часа с bias 59) и специфические строки ошибок (например, "YouShallNotPass") реализованы в коде и описаны в исходниках.
