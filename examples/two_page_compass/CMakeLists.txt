add_library(two_page_compass_app)

target_sources(two_page_compass_app PRIVATE
    src/app.cpp
)

target_compile_features(two_page_compass_app PUBLIC cxx_std_20)

target_compile_options(two_page_compass_app
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/WX>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wshadow -Wpedantic -Werror -Wno-unused-parameter>
)

target_include_directories(two_page_compass_app
    PUBLIC
        include
)

target_link_libraries(two_page_compass_app
    PUBLIC
        KazDragon::munin
)

add_executable(two_page_compass)

target_sources(two_page_compass PRIVATE
    src/main.cpp
)

target_compile_features(two_page_compass PRIVATE cxx_std_20)

target_compile_options(two_page_compass
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/WX>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wshadow -Wpedantic -Werror -Wno-unused-parameter>
)

target_link_libraries(two_page_compass
    PRIVATE
        two_page_compass_app
        KazDragon::consolepp
        Threads::Threads
)

add_executable(two_page_compass_test)

target_sources(two_page_compass_test PRIVATE
    test/two_page_compass_test.cpp
)

target_compile_features(two_page_compass_test PRIVATE cxx_std_20)

target_compile_options(two_page_compass_test
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/WX>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wshadow -Wpedantic -Werror -Wno-unused-parameter>
)

target_include_directories(two_page_compass_test
    PRIVATE
        include
)

target_link_libraries(two_page_compass_test
    PRIVATE
        GTest::gtest_main
        two_page_compass_app
        hugin
)

include(GoogleTest)
gtest_discover_tests(two_page_compass_test)
