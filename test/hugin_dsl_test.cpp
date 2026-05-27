#include <gtest/gtest.h>
#include <hugin/session.hpp>
#include <munin/button.hpp>
#include <munin/window.hpp>
#include <terminalpp/core.hpp>
#include <terminalpp/terminal.hpp>

#include <functional>

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

}  // namespace

TEST(
    hugin_dsl_session,
    queries_one_named_button_node_by_role_from_a_munin_window)
{
    fake_channel channel;
    terminalpp::terminal terminal{channel};
    auto content = munin::make_button(" OK ");
    munin::window window{terminal, content};

    hugin::session ui{window};

    auto const buttons = ui.query(hugin::by::role("button"));

    ASSERT_EQ(1U, buttons.size());
    EXPECT_EQ("button", buttons.front().role());
    EXPECT_EQ("OK", buttons.front().name());
}
