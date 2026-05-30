#include <hugin/session.hpp>
#include <munin/window.hpp>
#include <terminalpp/mouse.hpp>

#include <utility>

namespace hugin {

node::node(nlohmann::json snapshot, click_function click)
  : snapshot_(std::move(snapshot)), click_(std::move(click))
{
}

auto node::role() const -> std::string
{
    return snapshot_.value("type", "");
}

auto node::name() const -> std::string
{
    return snapshot_.value("name", "");
}

void node::click() const
{
    if (click_)
    {
        click_(click_position());
    }
}

auto node::click_position() const -> terminalpp::point
{
    return {
        snapshot_.at("position").at("x").get<terminalpp::coordinate_type>(),
        snapshot_.at("position").at("y").get<terminalpp::coordinate_type>()};
}

session::session(munin::window &window) : window_(window)
{
}

auto session::content_snapshot() const -> nlohmann::json
{
    return window_.to_json().at("content");
}

auto session::make_node(nlohmann::json snapshot) const -> node
{
    return node{std::move(snapshot), [this](terminalpp::point const &position) {
                    click_at(position);
                }};
}

void session::click_at(terminalpp::point const &position) const
{
    window_.event(
        terminalpp::mouse::event{
            terminalpp::mouse::event_type::left_button_down, position});
}

}  // namespace hugin
