cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

option(HUGIN_COVERAGE "Build with code coverage options" False)
option(HUGIN_WITH_TESTS "Build with tests" True)
option(HUGIN_WITH_EXAMPLES "Build examples" False)
option(HUGIN_DOC_ONLY "Build only documentation" False)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

if (HUGIN_VERSION)
    include(function-parse_version)
    parse_version("${HUGIN_VERSION}" "HUGIN_")
else()
    include(function-git_version)
    git_version(HUGIN)
endif()

message(STATUS "Hugin Version: ${HUGIN_VERSION}")
project(hugin VERSION ${HUGIN_VERSION} LANGUAGES CXX)

find_package(Doxygen)

if (DOXYGEN_FOUND)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )

    add_custom_target(hugin_doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT
            "Generate API documentation with Doxygen" VERBATIM
    )
endif()

if (HUGIN_DOC_ONLY)
    return()
endif()

find_package(Boost 1.69.0 REQUIRED)
find_package(nlohmann_json 3.3.0 CONFIG REQUIRED)

if (NOT TARGET KazDragon::terminalpp)
    find_package(terminalpp 4.0.0 CONFIG REQUIRED)
endif()

if (NOT TARGET KazDragon::munin)
    find_package(munin 0.7.1 CONFIG REQUIRED)
endif()

if (HUGIN_WITH_TESTS OR HUGIN_WITH_EXAMPLES)
    find_package(GTest CONFIG REQUIRED)
endif()

if (HUGIN_WITH_EXAMPLES)
    find_package(Threads REQUIRED)

    if (NOT TARGET KazDragon::consolepp)
        find_package(consolepp 0.1.1 CONFIG REQUIRED)
    endif()
endif()

if (HUGIN_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
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
        src/diagnostics.cpp
        src/selectors.cpp
        src/session.cpp
)

target_link_libraries(hugin PUBLIC KazDragon::munin)

if (HUGIN_WITH_TESTS)
    add_subdirectory(test)
endif()

if (HUGIN_WITH_EXAMPLES)
    add_subdirectory(examples)
endif()
