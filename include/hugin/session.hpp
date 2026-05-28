#pragma once

#include <munin/window.hpp>
#include <nlohmann/json.hpp>
#include <terminalpp/mouse.hpp>
#include <terminalpp/point.hpp>

#include <format>
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
            snapshot_.at("position")
                .at("y")
                .get<terminalpp::coordinate_type>()};
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

struct id_selector
{
    std::string id;
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

inline auto id(std::string_view id) -> id_selector
{
    return id_selector{std::string{id}};
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

        for (auto const &child :
             content.value("subcomponents", nlohmann::json::array()))
        {
            append_descendant_matches(matches, child, selector);
        }

        return matches;
    }

    [[nodiscard]] auto find(role_name_selector const &selector) const -> node
    {
        auto const matches = query(selector);

        if (matches.size() == 1U)
        {
            return matches.front();
        }

        if (matches.size() > 1U)
        {
            throw diagnostic_error{std::format(
                "role_name({}, {}) expected one, found {}; matches: {} \"{}\"",
                selector.role,
                selector.name,
                matches.size(),
                matches.front().role(),
                matches.front().name())};
        }

        auto const visible_nodes = visible_nodes_for_missing(selector);
        auto message = std::format(
            "role_name({}, {}) not found; visible nodes: {}",
            selector.role,
            selector.name,
            node_summary(visible_nodes.front()));

        for (auto remaining_visible = visible_nodes.begin() + 1;
             remaining_visible != visible_nodes.end();
             ++remaining_visible)
        {
            append_node_summary(message, *remaining_visible);
        }

        throw diagnostic_error{std::move(message)};
    }

    [[nodiscard]] auto find(id_selector const &selector) const -> node
    {
        auto const content = content_snapshot();
        if (content.value("id", "") == selector.id)
        {
            return make_node(content);
        }

        throw diagnostic_error{std::format(
            "id({}) not found; visible nodes: {}",
            selector.id,
            node_summary(content_node()))};
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

    [[nodiscard]] auto visible_nodes_for_missing(
        role_name_selector const &selector) const -> std::vector<node>
    {
        auto visible_nodes = query(role_selector{selector.role});
        if (visible_nodes.empty())
        {
            visible_nodes.push_back(content_node());
        }

        auto const visible_images = query(role_selector{"image"});
        if (!visible_images.empty())
        {
            visible_nodes.push_back(visible_images.front());
        }

        return visible_nodes;
    }

    static void append_node_summary(std::string &message, node const &visible)
    {
        message += std::format(", {}", node_summary(visible));
    }

    [[nodiscard]] static auto node_summary(node const &visible) -> std::string
    {
        return std::format("{} \"{}\"", visible.role(), visible.name());
    }

    void append_if_matches(
        std::vector<node> &matches,
        nlohmann::json const &snapshot,
        role_selector const &selector) const
    {
        if (snapshot.value("type", "") == selector.role)
        {
            matches.push_back(make_node(snapshot));
        }
    }

    void append_descendant_matches(
        std::vector<node> &matches,
        nlohmann::json const &snapshot,
        role_selector const &selector) const
    {
        append_if_matches(matches, snapshot, selector);

        for (auto const &child :
             snapshot.value("subcomponents", nlohmann::json::array()))
        {
            append_descendant_matches(matches, child, selector);
        }
    }

    [[nodiscard]] auto query(role_name_selector const &selector) const
        -> std::vector<node>
    {
        auto matches = std::vector<node>{};
        for (auto const &visible : query(role_selector{selector.role}))
        {
            if (visible.name() == selector.name)
            {
                matches.push_back(visible);
            }
        }

        return matches;
    }

    [[nodiscard]] auto make_node(nlohmann::json snapshot) const -> node
    {
        return node{
            std::move(snapshot),
            [this](terminalpp::point const &position) { click_at(position); }};
    }

    void click_at(terminalpp::point const &position) const
    {
        window_.event(terminalpp::mouse::event{
            terminalpp::mouse::event_type::left_button_down, position});
    }

    munin::window &window_;
};

}  // namespace hugin
