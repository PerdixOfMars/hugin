add_executable(hugin_test
    smoke_test.cpp
)

target_compile_features(hugin_test PRIVATE cxx_std_20)

target_compile_options(hugin_test
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/WX>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wshadow -Wpedantic -Werror>
)

add_test(
    NAME hugin_smoke.builds_and_runs
    COMMAND hugin_test
)
