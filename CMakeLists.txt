cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(hugin LANGUAGES CXX)

include(CTest)

if (BUILD_TESTING)
    set(TERMINALPP_WITH_TESTS FALSE CACHE BOOL "" FORCE)
    add_subdirectory(
        "${CMAKE_CURRENT_SOURCE_DIR}/../terminalpp"
        "${CMAKE_CURRENT_BINARY_DIR}/external/terminalpp"
    )

    set(MUNIN_WITH_CONSOLEPP FALSE CACHE BOOL "" FORCE)
    set(MUNIN_WITH_TESTS FALSE CACHE BOOL "" FORCE)
    add_subdirectory(
        "${CMAKE_CURRENT_SOURCE_DIR}/../munin"
        "${CMAKE_CURRENT_BINARY_DIR}/external/munin"
    )

    add_subdirectory(test)
endif()
