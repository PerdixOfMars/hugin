#pragma once

#include <munin/window.hpp>
#include <nlohmann/json.hpp>
#include <terminalpp/mouse.hpp>
#include <terminalpp/point.hpp>

#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace hugin {

class node
{
public:
    using click_function = std::function<void(terminalpp::point const &)>;

    explicit node(nlohmann::json snapshot, click_function click = {})
      : snapshot_(std::move(snapshot)), click_(std::move(click))
    {
    }

    [[nodiscard]] auto role() const -> std::string
    {
        return snapshot_.value("type", "");
    }

    [[nodiscard]] auto name() const -> std::string
    {
        return snapshot_.value("name", "");
    }

    void click() const
    {
        if (click_)
        {
            click_(click_position());
        }
    }

private:
    [[nodiscard]] auto click_position() const -> terminalpp::point
    {
        return {
            snapshot_.at("position").at("x").get<terminalpp::coordinate_type>(),
            snapshot_.at("position").at("y").get<terminalpp::coordinate_type>()};
    }

    nlohmann::json snapshot_;
    click_function click_;
};

struct role_selector
{
    std::string role;
};

struct role_name_selector
{
    std::string role;
    std::string name;
};

class diagnostic_error : public std::runtime_error
{
public:
    explicit diagnostic_error(std::string message)
      : std::runtime_error(std::move(message))
    {
    }
};

namespace by {

inline auto role(std::string_view role) -> role_selector
{
    return role_selector{std::string{role}};
}

inline auto role_name(std::string_view role, std::string_view name)
    -> role_name_selector
{
    return role_name_selector{std::string{role}, std::string{name}};
}

}  // namespace by

class session
{
public:
    explicit session(munin::window &window) : window_(window)
    {
    }

    [[nodiscard]] auto query(role_selector const &selector) const
        -> std::vector<node>
    {
        auto const content = content_snapshot();
        if (content.value("type", "") == selector.role)
        {
            return {make_node(content)};
        }

        auto matches = std::vector<node>{};
        for (auto const &child : content.at("subcomponents"))
        {
            if (child.value("type", "") == selector.role)
            {
                matches.push_back(make_node(child));
            }
        }

        return matches;
    }

    [[nodiscard]] auto find(role_name_selector const &selector) const -> node
    {
        auto const nodes = query(role_selector{selector.role});
        for (auto const &visible : nodes)
        {
            if (visible.name() == selector.name)
            {
                return visible;
            }
        }

        auto const visible = content_node();
        throw diagnostic_error{
            "role_name(" + selector.role + ", " + selector.name
            + ") not found; visible nodes: " + visible.role() + " \""
            + visible.name() + "\""};
    }

private:
    [[nodiscard]] auto content_node() const -> node
    {
        return make_node(content_snapshot());
    }

    [[nodiscard]] auto content_snapshot() const -> nlohmann::json
    {
        return window_.to_json().at("content");
    }

    [[nodiscard]] auto make_node(nlohmann::json snapshot) const -> node
    {
        return node{
            std::move(snapshot),
            [this](terminalpp::point const &position) {
                click_at(position);
            }};
    }

    void click_at(terminalpp::point const &position) const
    {
        window_.event(terminalpp::mouse::event{
            terminalpp::mouse::event_type::left_button_down, position});
    }

    munin::window &window_;
};

}  // namespace hugin
