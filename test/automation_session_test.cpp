#include <hugin/automation_session.hpp>

#include <gtest/gtest.h>
#include <munin/basic_component.hpp>
#include <munin/render_surface.hpp>
#include <terminalpp/virtual_key.hpp>

#include <any>
#include <optional>

namespace {

class recording_component : public munin::basic_component
{
public:
    [[nodiscard]] auto last_key() const
        -> std::optional<terminalpp::virtual_key>
    {
        return last_key_;
    }

private:
    [[nodiscard]] auto do_get_preferred_size() const
        -> terminalpp::extent override
    {
        return {};
    }

    void do_draw(
        munin::render_surface &,
        terminalpp::rectangle const &) const override
    {
    }

    void do_event(std::any const &event) override
    {
        if (auto const *key = std::any_cast<terminalpp::virtual_key>(&event))
        {
            last_key_ = *key;
        }
    }

    std::optional<terminalpp::virtual_key> last_key_;
};

}  // namespace

TEST(automation_session_press_key, sends_the_key_to_the_inspected_root)
{
    recording_component root;
    hugin::automation_session session{root};
    auto const enter_key = terminalpp::virtual_key{
        terminalpp::vk::enter,
        terminalpp::vk_modifier::none,
        1,
        static_cast<terminalpp::byte>('\r')};

    session.press_key(enter_key);

    ASSERT_TRUE(root.last_key().has_value());
    EXPECT_EQ(terminalpp::vk::enter, root.last_key()->key);
}
