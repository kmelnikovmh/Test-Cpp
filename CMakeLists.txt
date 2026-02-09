cmake_minimum_required(VERSION 3.19)
project(test-cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Main app
add_executable(test-app
               src/main.cpp
               src/parser.cpp
               src/events.cpp
               src/club-state.cpp
               src/run-day.cpp)
               
target_compile_options(test-app PRIVATE -Wall
                                    -Wextra
                                    -Wpedantic
                                    -pedantic-errors
                                    -Werror)
