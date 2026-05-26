#pragma once

#include <munin/window.hpp>
#include <nlohmann/json.hpp>

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

private:
    nlohmann::json snapshot_;
};

struct role_selector
{
    std::string role;
};

namespace by {

inline auto role(std::string_view role) -> role_selector
{
    return role_selector{std::string{role}};
}

}  // namespace by

class session
{
public:
    explicit session(munin::window &window) : window_(window)
    {
    }

    [[nodiscard]] auto query(role_selector const &) const -> std::vector<node>
    {
        return {node{window_.to_json().at("content")}};
    }

private:
    munin::window &window_;
};

}  // namespace hugin
