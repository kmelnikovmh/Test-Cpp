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


# GTest,  UBSan/ASan, Clang-Tidy
enable_testing()
find_package(GTest REQUIRED)
find_program(CLANG_TIDY_EXE NAMES clang-tidy)

function(add_gtest TEST_NAME TEST_SRC)
    add_executable(${TEST_NAME} ${TEST_SRC} src/parser.cpp src/events.cpp src/club-state.cpp src/run-day.cpp)
    target_link_libraries(${TEST_NAME} GTest::gtest_main)
    set_target_properties(${TEST_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)

    if(CLANG_TIDY_EXE)
        add_custom_command(TARGET ${TEST_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Running clang-tidy for ${TEST_NAME}"
        )
        set_target_properties(${TEST_NAME} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-header-filter=^${PROJECT_SOURCE_DIR}/"
        )
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${TEST_NAME} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${TEST_NAME} PRIVATE -fsanitize=address,undefined)
    endif()

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

add_gtest(parser-test test/parser-test.cpp)
add_gtest(club-test test/club-test.cpp)
add_gtest(events-test test/events-test.cpp)
add_gtest(run-day-test test/run-day-test.cpp)

add_custom_target(execute-tests
    COMMAND env GTEST_COLOR=1 ${CMAKE_CTEST_COMMAND} --verbose --output-on-failure
    DEPENDS parser-test club-test events-test run-day-test
)