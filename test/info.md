## Покрытие тестами

Все тесты используют Google Test Framework

### Parser Tests - 6 тестов
- ParseClubValidConfig - парсинг конфига с корректными данными
- ParseEventsValidAndMultiple - парсинг нескольких событий разных типов
- ParseEventsErrorInvalidTable - валидация номера стола
- ParseEventsErrorChronological - проверка хронологического порядка событий
- ParseEventsErrorTimeFormatAndName - проверка формата времени и имён клиентов
- ComplexScenarioFullDay - комплексный сценарий с границами времени

### ClubState Tests - 4 теста
- TimeAndClientBasics - проверка времени работы и управление клиентами
- SitGetUpAndRound - посадка, вставание и округление оплаты
- TablesAndQueue - управление столами и очередью ожидания
- QueueOpsAndErrors - операции с очередью и проверка исключений

### Events Tests - 5 тестов
- ArriveAndSit - события прибытия и посадки за стол
- ArriveErrors - проверка ошибок прибытия (до открытия, дубликат)
- SitErrorsAndChange - ошибки посадки и смена столов
- WaitAndOverflow - событие ожидания и переполнение очереди
- LeftAndSystemSit - уход клиента и генерация системного события посадки

### RunDay Tests - 5 тестов
- EmptyDay - пустой рабочий день
- BasicFlowAndErrors - базовый цикл клиента и обработка ошибок
- BusyTableAndQueue - сценарии занятых столов и очереди
- EndOfDaySystemLeft - генерация системных событий ухода в конце дня
- ErrorsDoNotCrash - обработка ошибок не нарушает состояние системы

## Итого: 20 юнит тестов
