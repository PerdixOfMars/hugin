#include <hugin/session.hpp>
#include <munin/window.hpp>
#include <terminalpp/mouse.hpp>

#include <utility>

namespace hugin {

node::node(
    nlohmann::json snapshot,
    click_function click,
    terminalpp::point click_position)
  : snapshot_(std::move(snapshot)),
    click_(std::move(click)),
    click_position_(click_position)
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
    return click_position_;
}

session::session(munin::window &window) : window_(window)
{
}

auto session::content_snapshot() const -> nlohmann::json
{
    return window_.to_json().at("content");
}

auto session::snapshot_position(nlohmann::json const &snapshot)
    -> terminalpp::point
{
    return {
        snapshot.at("position").at("x").get<terminalpp::coordinate_type>(),
        snapshot.at("position").at("y").get<terminalpp::coordinate_type>()};
}

auto session::make_node(
    nlohmann::json snapshot, terminalpp::point click_position) const -> node
{
    return node{
        std::move(snapshot),
        [this](terminalpp::point const &position) { click_at(position); },
        click_position};
}

void session::click_at(terminalpp::point const &position) const
{
    window_.event(terminalpp::mouse::event{
        terminalpp::mouse::event_type::left_button_down, position});
}

}  // namespace hugin
