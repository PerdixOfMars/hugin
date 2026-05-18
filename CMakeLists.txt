cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(hugin LANGUAGES CXX)

include(CTest)

add_library(hugin INTERFACE)

target_compile_features(hugin INTERFACE cxx_std_20)

target_include_directories(hugin
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

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
