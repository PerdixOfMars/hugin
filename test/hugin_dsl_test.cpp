#include <gtest/gtest.h>
#include <hugin/session.hpp>
#include <munin/button.hpp>
#include <munin/container.hpp>
#include <munin/window.hpp>
#include <terminalpp/core.hpp>
#include <terminalpp/terminal.hpp>

#include <functional>
#include <memory>
#include <string>

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

TEST(
    hugin_dsl_session,
    strict_find_reports_missing_button_with_a_diagnostic)
{
    single_button_window screen;

    try
    {
        (void)screen.ui.find(hugin::by::role_name("button", "Cancel"));
        FAIL() << "Expected strict find to throw for a missing button";
    }
    catch (hugin::diagnostic_error const &error)
    {
        auto const message = std::string{error.what()};
        EXPECT_NE(
            std::string::npos,
            message.find("role_name(button, Cancel)"));
        EXPECT_NE(std::string::npos, message.find("button \"OK\""));
    }
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
    content->add_component(munin::make_button(" OK "));
    munin::window window{terminal, content};
    hugin::session ui{window};

    auto const buttons = ui.query(hugin::by::role("button"));

    ASSERT_EQ(1U, buttons.size());
    EXPECT_EQ("button", buttons.front().role());
}
