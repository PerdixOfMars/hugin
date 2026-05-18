cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(hugin LANGUAGES CXX)

include(CTest)

if (BUILD_TESTING)
    add_subdirectory(test)
endif()
