#include <hugin/munin_snapshot.hpp>
#include <hugin/snapshot_queries.hpp>

#include <gtest/gtest.h>
#include <munin/button.hpp>
#include <munin/container.hpp>

namespace {

auto make_focused_container_snapshot() -> nlohmann::json
{
    munin::container container;
    auto button = munin::make_button(" OK ");
    button->set_id("ok_button");
    container.add_component(button);
    container.set_focus();

    return hugin::capture_snapshot(container);
}

}  // namespace

TEST(live_snapshot_capture, preserves_root_type_for_a_live_munin_container)
{
    auto const snapshot = make_focused_container_snapshot();

    EXPECT_EQ("container", snapshot["type"]);
}

TEST(live_snapshot_queries, counts_button_components_in_a_live_snapshot)
{
    auto const snapshot = make_focused_container_snapshot();

    EXPECT_EQ(1U, hugin::count_components_of_type(snapshot, "button"));
}

TEST(live_snapshot_queries, returns_the_button_as_the_focused_leaf)
{
    auto const snapshot = make_focused_container_snapshot();

    auto const focused_leaf = hugin::find_focused_leaf(snapshot);

    ASSERT_TRUE(focused_leaf.has_value());
    EXPECT_EQ("button", (*focused_leaf)["type"]);
}

TEST(live_snapshot_queries, finds_a_component_by_automation_id)
{
    auto const snapshot = make_focused_container_snapshot();

    auto const component = hugin::find_component_by_id(snapshot, "ok_button");

    ASSERT_TRUE(component.has_value());
    EXPECT_EQ("button", (*component)["type"]);
}
