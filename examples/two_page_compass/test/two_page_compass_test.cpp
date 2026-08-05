#include <gtest/gtest.h>
#include <hugin/session.hpp>
#include <munin/window.hpp>
#include <terminalpp/terminal.hpp>
#include <two_page_compass/app.hpp>

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

class two_page_compass_example : public testing::Test
{
protected:
    auto make_session() -> hugin::session
    {
        content = two_page_compass::make_content();
        content->set_size({79, 23});
        window = std::make_unique<munin::window>(terminal, content);
        return hugin::session{*window};
    }

    fake_channel channel;
    terminalpp::terminal terminal{channel};
    std::shared_ptr<munin::component> content;
    std::unique_ptr<munin::window> window;
};

}  // namespace

TEST_F(two_page_compass_example, starts_on_page_one_with_a_next_button)
{
    auto ui = make_session();

    EXPECT_EQ(
        "Page One", ui.find(hugin::by::role_name("image", "Page One")).name());
    EXPECT_EQ("Next", ui.find(hugin::by::role_name("button", "Next")).name());
}
