#include <gtest/gtest.h>
#include <hugin/session.hpp>
#include <munin/button.hpp>
#include <munin/container.hpp>
#include <munin/image.hpp>
#include <munin/vertical_strip_layout.hpp>
#include <munin/window.hpp>
#include <terminalpp/core.hpp>
#include <terminalpp/terminal.hpp>

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <cstddef>

namespace {

struct fake_channel
{
    void async_read(std::function<void(terminalpp::bytes)> const &callback)
    {
        read_callback = callback;
    }

    void write(terminalpp::bytes data)
    {
        written.append(data.begin(), data.end());
    }

    [[nodiscard]] bool is_alive() const
    {
        return alive;
    }

    void close()
    {
        alive = false;

        if (read_callback)
        {
            read_callback({});
        }
    }

    std::function<void(terminalpp::bytes)> read_callback;
    terminalpp::byte_storage written;
    bool alive{true};
};

struct single_button_window
{
    single_button_window()
      : terminal(channel),
        content(munin::make_button(" OK ")),
        window(terminal, content),
        ui(window)
    {
    }

    fake_channel channel;
    terminalpp::terminal terminal;
    std::shared_ptr<munin::button> content;
    munin::window window;
    hugin::session ui;
};

void add_button_under_intermediate_containers(
    std::shared_ptr<munin::container> const &content,
    std::size_t intermediate_container_count)
{
    auto parent = content;
    for (auto count = std::size_t{}; count != intermediate_container_count;
         ++count)
    {
        auto child = std::make_shared<munin::container>();
        parent->add_component(child);
        parent = child;
    }

    parent->add_component(munin::make_button(" OK "));
}

void expect_missing_button_diagnostic_contains(
    hugin::session &ui,
    std::initializer_list<std::string_view> visible_node_summaries)
{
    try
    {
        (void)ui.find(hugin::by::role_name("button", "Cancel"));
        FAIL() << "Expected strict find to throw for a missing button";
    }
    catch (hugin::diagnostic_error const &error)
    {
        auto const message = std::string{error.what()};
        EXPECT_NE(std::string::npos, message.find("role_name(button, Cancel)"));
        for (auto const visible_node_summary : visible_node_summaries)
        {
            EXPECT_NE(std::string::npos, message.find(visible_node_summary));
        }
    }
}

void expect_missing_button_diagnostic_contains(
    hugin::session &ui, std::string_view visible_node_summary)
{
    expect_missing_button_diagnostic_contains(ui, {visible_node_summary});
}

}  // namespace

TEST(
    hugin_dsl_session,
    queries_one_named_button_node_by_role_from_a_munin_window)
{
    single_button_window screen;

    auto const buttons = screen.ui.query(hugin::by::role("button"));

    ASSERT_EQ(1U, buttons.size());
    EXPECT_EQ("button", buttons.front().role());
    EXPECT_EQ("OK", buttons.front().name());
}

TEST(hugin_dsl_session, strict_find_reports_missing_button_with_a_diagnostic)
{
    single_button_window screen;

    expect_missing_button_diagnostic_contains(screen.ui, "button \"OK\"");
}

TEST(
    hugin_dsl_session,
    strict_find_reports_a_grandchild_button_in_missing_diagnostics)
{
    fake_channel channel;
    terminalpp::terminal terminal{channel};
    auto content = std::make_shared<munin::container>();
    add_button_under_intermediate_containers(content, 1U);
    munin::window window{terminal, content};
    hugin::session ui{window};

    expect_missing_button_diagnostic_contains(ui, "button \"OK\"");
}

TEST(
    hugin_dsl_session,
    strict_find_reports_a_sibling_image_in_missing_diagnostics)
{
    fake_channel channel;
    terminalpp::terminal terminal{channel};
    auto content = std::make_shared<munin::container>();
    content->add_component(munin::make_button(" OK "));
    content->add_component(munin::make_image("Ready"));
    munin::window window{terminal, content};
    hugin::session ui{window};

    expect_missing_button_diagnostic_contains(
        ui, {"button \"OK\"", "image \"Ready\""});
}

TEST(hugin_dsl_session, strict_find_returns_a_button_matching_role_and_name)
{
    single_button_window screen;

    auto const button = screen.ui.find(hugin::by::role_name("button", "OK"));

    EXPECT_EQ("button", button.role());
    EXPECT_EQ("OK", button.name());
}

TEST(hugin_dsl_session, queries_a_nested_button_node_by_role)
{
    fake_channel channel;
    terminalpp::terminal terminal{channel};
    auto content = std::make_shared<munin::container>();
    add_button_under_intermediate_containers(content, 0U);
    munin::window window{terminal, content};
    hugin::session ui{window};

    auto const buttons = ui.query(hugin::by::role("button"));

    ASSERT_EQ(1U, buttons.size());
    EXPECT_EQ("button", buttons.front().role());
}

TEST(hugin_dsl_session, queries_a_grandchild_button_node_by_role)
{
    fake_channel channel;
    terminalpp::terminal terminal{channel};
    auto content = std::make_shared<munin::container>();
    add_button_under_intermediate_containers(content, 1U);
    munin::window window{terminal, content};
    hugin::session ui{window};

    auto const buttons = ui.query(hugin::by::role("button"));

    ASSERT_EQ(1U, buttons.size());
    EXPECT_EQ("button", buttons.front().role());
}

TEST(hugin_dsl_session, queries_a_great_grandchild_button_node_by_role)
{
    fake_channel channel;
    terminalpp::terminal terminal{channel};
    auto content = std::make_shared<munin::container>();
    add_button_under_intermediate_containers(content, 2U);
    munin::window window{terminal, content};
    hugin::session ui{window};

    auto const buttons = ui.query(hugin::by::role("button"));

    ASSERT_EQ(1U, buttons.size());
    EXPECT_EQ("button", buttons.front().role());
}

TEST(hugin_dsl_session, clicking_button_changes_sibling_image_name)
{
    fake_channel channel;
    terminalpp::terminal terminal{channel};
    auto content = std::make_shared<munin::container>();
    content->set_layout(munin::make_vertical_strip_layout());
    auto status = munin::make_image("Before");
    auto button = munin::make_button(" OK ");
    button->on_click.connect([status] { status->set_content("After"); });
    content->add_component(status);
    content->add_component(button);
    content->set_size({20, 3});
    munin::window window{terminal, content};
    hugin::session ui{window};

    ui.find(hugin::by::role_name("button", "OK")).click();

    auto const image = ui.find(hugin::by::role_name("image", "After"));
    EXPECT_EQ("After", image.name());
}

TEST(hugin_dsl_session, strict_find_reports_ambiguous_button_with_a_diagnostic)
{
    fake_channel channel;
    terminalpp::terminal terminal{channel};
    auto content = std::make_shared<munin::container>();
    content->add_component(munin::make_button(" OK "));
    content->add_component(munin::make_button(" OK "));
    munin::window window{terminal, content};
    hugin::session ui{window};

    try
    {
        (void)ui.find(hugin::by::role_name("button", "OK"));
        FAIL() << "Expected strict find to throw for ambiguous buttons";
    }
    catch (hugin::diagnostic_error const &error)
    {
        auto const message = std::string{error.what()};
        EXPECT_NE(std::string::npos, message.find("expected one, found 2"));
        EXPECT_NE(std::string::npos, message.find("button \"OK\""));
    }
}
