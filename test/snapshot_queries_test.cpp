#include <hugin/snapshot_queries.hpp>

#include <gtest/gtest.h>

TEST(snapshot_query_count_components_of_type, returns_zero_when_the_root_type_does_not_match)
{
    auto const snapshot = nlohmann::json{
        {"type", "container"},
    };

    EXPECT_EQ(0U, hugin::count_components_of_type(snapshot, "button"));
}
