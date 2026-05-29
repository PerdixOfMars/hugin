
add_executable(hugin_test)

target_sources(hugin_test PRIVATE
    automation_session_test.cpp
    hugin_dsl_test.cpp
    live_snapshot_query_test.cpp
    snapshot_queries_test.cpp
)

target_compile_features(hugin_test PRIVATE cxx_std_20)

target_compile_options(hugin_test
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/WX>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wshadow -Wpedantic -Werror -Wno-unused-parameter>
)

target_link_libraries(hugin_test
    PRIVATE
        GTest::gtest_main
        hugin
)

include(GoogleTest)
gtest_discover_tests(hugin_test)
