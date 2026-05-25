#pragma once

#include <munin/component.hpp>
#include <terminalpp/virtual_key.hpp>

namespace hugin {

class automation_session
{
public:
    explicit automation_session(munin::component &root);

    [[nodiscard]] auto snapshot() const -> nlohmann::json;

    void press_key(terminalpp::virtual_key const &key);

private:
    munin::component &root_;
};

}  // namespace hugin
