add_executable(hugin_test
    live_snapshot_query_test.cpp
)

target_compile_features(hugin_test PRIVATE cxx_std_20)

target_compile_options(hugin_test
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/WX>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wshadow -Wpedantic -Werror -Wno-unused-parameter>
)

target_link_libraries(hugin_test
    PRIVATE
        hugin
        KazDragon::munin
)

add_test(
    NAME hugin_live_snapshot_queries.capture_and_count
    COMMAND hugin_test
)
