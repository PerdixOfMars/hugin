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

TEST(snapshot_query_find_component_by_id, returns_empty_when_the_root_does_not_match)
{
    auto const snapshot = nlohmann::json{
        {"type", "container"},
        {"id", "main_screen"},
    };

    EXPECT_FALSE(hugin::find_component_by_id(snapshot, "ok_button").has_value());
}

TEST(snapshot_query_find_focused_leaf, returns_the_focused_root)
{
    auto const snapshot = nlohmann::json{
        {"type", "button"},
        {"has_focus", true},
    };

    auto const focused_leaf = hugin::find_focused_leaf(snapshot);

    ASSERT_TRUE(focused_leaf.has_value());
    EXPECT_EQ("button", (*focused_leaf)["type"]);
}

TEST(snapshot_query_find_component_by_id, returns_the_matching_root)
{
    auto const snapshot = nlohmann::json{
        {"type", "button"},
        {"id", "ok_button"},
    };

    auto const component = hugin::find_component_by_id(snapshot, "ok_button");

    ASSERT_TRUE(component.has_value());
    EXPECT_EQ("button", (*component)["type"]);
}

TEST(snapshot_query_find_focused_leaf, returns_a_focused_child_instead_of_the_parent)
{
    auto const snapshot = nlohmann::json{
        {"type", "container"},
        {"has_focus", true},
        {"subcomponents",
         nlohmann::json::array({nlohmann::json{
             {"type", "button"},
             {"has_focus", true},
         }})},
    };

    auto const focused_leaf = hugin::find_focused_leaf(snapshot);

    ASSERT_TRUE(focused_leaf.has_value());
    EXPECT_EQ("button", (*focused_leaf)["type"]);
}

TEST(snapshot_query_find_component_by_id, returns_a_matching_child)
{
    auto const snapshot = nlohmann::json{
        {"type", "container"},
        {"id", "main_screen"},
        {"subcomponents",
         nlohmann::json::array({nlohmann::json{
             {"type", "button"},
             {"id", "ok_button"},
         }})},
    };

    auto const component = hugin::find_component_by_id(snapshot, "ok_button");

    ASSERT_TRUE(component.has_value());
    EXPECT_EQ("button", (*component)["type"]);
}

TEST(snapshot_query_count_components_of_type, counts_multiple_matches_across_the_tree)
{
    auto const snapshot = nlohmann::json{
        {"type", "container"},
        {"subcomponents",
         nlohmann::json::array(
             {nlohmann::json{{"type", "button"}},
              nlohmann::json{{"type", "label"}},
              nlohmann::json{{"type", "button"}}})},
    };

    EXPECT_EQ(2U, hugin::count_components_of_type(snapshot, "button"));
}
