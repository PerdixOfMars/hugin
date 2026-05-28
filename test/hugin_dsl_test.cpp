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

struct dsl_screen
{
    template <typename Component>
    dsl_screen(
        terminalpp::terminal &terminal,
        std::shared_ptr<Component> const &content)
      : window(terminal, content), ui(window)
    {
    }

    munin::window window;
    hugin::session ui;
};

class hugin_dsl_session : public testing::Test
{
protected:
    [[nodiscard]] auto screen_with_single_button() -> dsl_screen
    {
        return screen_with(munin::make_button(" OK "));
    }

    template <typename Component>
    [[nodiscard]] auto screen_with(std::shared_ptr<Component> const &content)
        -> dsl_screen
    {
        return dsl_screen{terminal, content};
    }

    static void add_button_under_intermediate_containers(
        std::shared_ptr<munin::container> const &content,
        std::size_t intermediate_container_count,
        std::string_view label)
    {
        auto parent = content;
        for (auto count = std::size_t{}; count != intermediate_container_count;
             ++count)
        {
            auto child = std::make_shared<munin::container>();
            parent->add_component(child);
            parent = child;
        }

        parent->add_component(munin::make_button(std::string{label}));
    }

    static void add_button_under_intermediate_containers(
        std::shared_ptr<munin::container> const &content,
        std::size_t intermediate_container_count)
    {
        add_button_under_intermediate_containers(
            content, intermediate_container_count, " OK ");
    }

    static void expect_missing_button_diagnostic_contains(
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
            EXPECT_NE(
                std::string::npos, message.find("role_name(button, Cancel)"));
            for (auto const visible_node_summary : visible_node_summaries)
            {
                EXPECT_NE(
                    std::string::npos, message.find(visible_node_summary));
            }
        }
    }

    static void expect_missing_button_diagnostic_contains(
        hugin::session &ui, std::string_view visible_node_summary)
    {
        expect_missing_button_diagnostic_contains(ui, {visible_node_summary});
    }

    fake_channel channel;
    terminalpp::terminal terminal{channel};
};

}  // namespace

TEST_F(
    hugin_dsl_session,
    queries_one_named_button_node_by_role_from_a_munin_window)
{
    auto screen = screen_with_single_button();

    auto const buttons = screen.ui.query(hugin::by::role("button"));

    ASSERT_EQ(1U, buttons.size());
    EXPECT_EQ("button", buttons.front().role());
    EXPECT_EQ("OK", buttons.front().name());
}

TEST_F(hugin_dsl_session, strict_find_reports_missing_button_with_a_diagnostic)
{
    auto screen = screen_with_single_button();

    expect_missing_button_diagnostic_contains(screen.ui, "button \"OK\"");
}

TEST_F(
    hugin_dsl_session,
    strict_find_reports_a_grandchild_button_in_missing_diagnostics)
{
    auto content = std::make_shared<munin::container>();
    add_button_under_intermediate_containers(content, 1U);
    auto screen = screen_with(content);

    expect_missing_button_diagnostic_contains(screen.ui, "button \"OK\"");
}

TEST_F(
    hugin_dsl_session,
    strict_find_reports_a_sibling_image_in_missing_diagnostics)
{
    auto content = std::make_shared<munin::container>();
    content->add_component(munin::make_button(" OK "));
    content->add_component(munin::make_image("Ready"));
    auto screen = screen_with(content);

    expect_missing_button_diagnostic_contains(
        screen.ui, {"button \"OK\"", "image \"Ready\""});
}

TEST_F(
    hugin_dsl_session, strict_find_reports_two_buttons_in_missing_diagnostics)
{
    auto content = std::make_shared<munin::container>();
    add_button_under_intermediate_containers(content, 0U, " OK ");
    add_button_under_intermediate_containers(content, 1U, " Save ");
    auto screen = screen_with(content);

    expect_missing_button_diagnostic_contains(
        screen.ui, {"button \"OK\"", "button \"Save\""});
}

TEST_F(hugin_dsl_session, strict_find_returns_a_button_matching_role_and_name)
{
    auto screen = screen_with_single_button();

    auto const button = screen.ui.find(hugin::by::role_name("button", "OK"));

    EXPECT_EQ("button", button.role());
    EXPECT_EQ("OK", button.name());
}

TEST_F(hugin_dsl_session, strict_find_returns_a_button_matching_automation_id)
{
    auto screen = screen_with(
        munin::make_button(" OK ") | munin::with_id("ok_button"));

    auto const found = screen.ui.find(hugin::by::id("ok_button"));

    EXPECT_EQ("button", found.role());
}

TEST_F(hugin_dsl_session, strict_find_reports_missing_automation_id_with_visible_nodes)
{
    auto screen = screen_with_single_button();

    try
    {
        (void)screen.ui.find(hugin::by::id("cancel_button"));
        FAIL() << "Expected strict find to throw for a missing Automation ID";
    }
    catch (hugin::diagnostic_error const &error)
    {
        auto const message = std::string{error.what()};
        EXPECT_NE(std::string::npos, message.find("id(cancel_button)"));
        EXPECT_NE(std::string::npos, message.find("button \"OK\""));
    }
}

TEST_F(hugin_dsl_session, strict_find_returns_a_child_button_matching_automation_id)
{
    auto content = std::make_shared<munin::container>();
    content->add_component(
        munin::make_button(" OK ") | munin::with_id("ok_button"));
    auto screen = screen_with(content);

    auto const found = screen.ui.find(hugin::by::id("ok_button"));

    EXPECT_EQ("button", found.role());
}

TEST_F(hugin_dsl_session, queries_a_nested_button_node_by_role)
{
    auto content = std::make_shared<munin::container>();
    add_button_under_intermediate_containers(content, 0U);
    auto screen = screen_with(content);

    auto const buttons = screen.ui.query(hugin::by::role("button"));

    ASSERT_EQ(1U, buttons.size());
    EXPECT_EQ("button", buttons.front().role());
}

TEST_F(hugin_dsl_session, queries_a_grandchild_button_node_by_role)
{
    auto content = std::make_shared<munin::container>();
    add_button_under_intermediate_containers(content, 1U);
    auto screen = screen_with(content);

    auto const buttons = screen.ui.query(hugin::by::role("button"));

    ASSERT_EQ(1U, buttons.size());
    EXPECT_EQ("button", buttons.front().role());
}

TEST_F(hugin_dsl_session, queries_a_great_grandchild_button_node_by_role)
{
    auto content = std::make_shared<munin::container>();
    add_button_under_intermediate_containers(content, 2U);
    auto screen = screen_with(content);

    auto const buttons = screen.ui.query(hugin::by::role("button"));

    ASSERT_EQ(1U, buttons.size());
    EXPECT_EQ("button", buttons.front().role());
}

TEST_F(hugin_dsl_session, clicking_button_changes_sibling_image_name)
{
    auto content = std::make_shared<munin::container>();
    content->set_layout(munin::make_vertical_strip_layout());
    auto status = munin::make_image("Before");
    auto button = munin::make_button(" OK ");
    button->on_click.connect([status] { status->set_content("After"); });
    content->add_component(status);
    content->add_component(button);
    content->set_size({20, 3});
    auto screen = screen_with(content);

    screen.ui.find(hugin::by::role_name("button", "OK")).click();

    auto const image = screen.ui.find(hugin::by::role_name("image", "After"));
    EXPECT_EQ("After", image.name());
}

TEST_F(
    hugin_dsl_session, strict_find_reports_ambiguous_button_with_a_diagnostic)
{
    auto content = std::make_shared<munin::container>();
    content->add_component(munin::make_button(" OK "));
    content->add_component(munin::make_button(" OK "));
    auto screen = screen_with(content);

    try
    {
        (void)screen.ui.find(hugin::by::role_name("button", "OK"));
        FAIL() << "Expected strict find to throw for ambiguous buttons";
    }
    catch (hugin::diagnostic_error const &error)
    {
        auto const message = std::string{error.what()};
        EXPECT_NE(std::string::npos, message.find("expected one, found 2"));
        EXPECT_NE(std::string::npos, message.find("button \"OK\""));
    }
}
