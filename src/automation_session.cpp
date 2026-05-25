#include "hugin/automation_session.hpp"

#include "hugin/munin_snapshot.hpp"

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

}  // namespace hugin
