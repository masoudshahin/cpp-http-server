cmake_minimum_required(VERSION 3.15)

project(HttpServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server
    src/main.cpp
)

target_include_directories(
    server
    PRIVATE
    include
)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests
    tests/HttpParserTests.cpp
    tests/UserStoreTests.cpp
    tests/RouterTests.cpp
)

target_include_directories(
    tests
    PRIVATE
    include
)

target_link_libraries(
    tests
    PRIVATE
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(tests)