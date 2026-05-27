#include "hugin/automation_session.hpp"

#include "hugin/munin_snapshot.hpp"

#include <terminalpp/mouse.hpp>

namespace hugin {

automation_session::automation_session(munin::component &root) : root_(root)
{
}

auto automation_session::snapshot() const -> nlohmann::json
{
    return capture_snapshot(root_);
}

void automation_session::press_key(terminalpp::virtual_key const &key)
{
    root_.event(key);
}

void automation_session::click(terminalpp::point const &position)
{
    root_.event(terminalpp::mouse::event{
        terminalpp::mouse::event_type::left_button_down, position});
}

}  // namespace hugin
