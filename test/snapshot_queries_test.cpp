#include <hugin/snapshot_queries.hpp>

#include <gtest/gtest.h>

TEST(snapshot_query_count_components_of_type, returns_zero_when_the_root_type_does_not_match)
{
    auto const snapshot = nlohmann::json{
        {"type", "container"},
    };

    EXPECT_EQ(0U, hugin::count_components_of_type(snapshot, "button"));
}

TEST(snapshot_query_count_components_of_type, counts_a_matching_root)
{
    auto const snapshot = nlohmann::json{
        {"type", "button"},
    };

    EXPECT_EQ(1U, hugin::count_components_of_type(snapshot, "button"));
}

TEST(snapshot_query_count_components_of_type, counts_one_nested_match)
{
    auto const snapshot = nlohmann::json{
        {"type", "container"},
        {"subcomponents",
         nlohmann::json::array(
             {nlohmann::json{{"type", "button"}}})},
    };

    EXPECT_EQ(1U, hugin::count_components_of_type(snapshot, "button"));
}

TEST(snapshot_query_find_focused_leaf, returns_empty_when_the_root_does_not_have_focus)
{
    auto const snapshot = nlohmann::json{
        {"type", "container"},
        {"has_focus", false},
    };

    EXPECT_FALSE(hugin::find_focused_leaf(snapshot).has_value());
}
