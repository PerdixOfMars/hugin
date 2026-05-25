cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(hugin LANGUAGES CXX)

find_package(GTest CONFIG REQUIRED)
find_package(Boost 1.69.0 REQUIRED)
find_package(nlohmann_json 3.3.0 CONFIG REQUIRED)

if (NOT TARGET KazDragon::consolepp)
    find_package(consolepp 0.1.0 CONFIG REQUIRED)
endif()

if (NOT TARGET KazDragon::terminalpp)
    find_package(terminalpp 4.0.0 CONFIG REQUIRED)
endif()

if (NOT TARGET KazDragon::munin)
    find_package(munin 0.7.1 CONFIG REQUIRED)
endif()

include(CTest)

add_library(hugin)

target_compile_features(hugin PUBLIC cxx_std_20)

target_include_directories(hugin
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(hugin
    PRIVATE
        src/automation_session.cpp
        src/munin_snapshot.cpp
        src/snapshot_queries.cpp
)

target_link_libraries(hugin PUBLIC KazDragon::munin)

add_subdirectory(test)
