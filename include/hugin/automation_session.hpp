#pragma once

#include <munin/component.hpp>
#include <terminalpp/point.hpp>
#include <terminalpp/virtual_key.hpp>

namespace hugin {

class automation_session
{
public:
    explicit automation_session(munin::component &root);

    [[nodiscard]] auto snapshot() const -> nlohmann::json;

    void press_key(terminalpp::virtual_key const &key);
    void click(terminalpp::point const &position);

private:
    munin::component &root_;
};

}  // namespace hugin
