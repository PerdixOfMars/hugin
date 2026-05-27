#pragma once

#include <munin/window.hpp>
#include <nlohmann/json.hpp>

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace hugin {

class node
{
public:
    explicit node(nlohmann::json snapshot) : snapshot_(std::move(snapshot))
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

private:
    nlohmann::json snapshot_;
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
            return {node{content}};
        }

        return {node{content.at("subcomponents").at(0)}};
    }

    [[nodiscard]] auto find(role_name_selector const &selector) const -> node
    {
        auto const visible = content_node();
        if (visible.role() == selector.role && visible.name() == selector.name)
        {
            return visible;
        }

        throw diagnostic_error{
            "role_name(" + selector.role + ", " + selector.name
            + ") not found; visible nodes: " + visible.role() + " \""
            + visible.name() + "\""};
    }

private:
    [[nodiscard]] auto content_node() const -> node
    {
        return node{content_snapshot()};
    }

    [[nodiscard]] auto content_snapshot() const -> nlohmann::json
    {
        return window_.to_json().at("content");
    }

    munin::window &window_;
};

}  // namespace hugin
