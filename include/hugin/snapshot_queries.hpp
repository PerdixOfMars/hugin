#pragma once

#include <nlohmann/json.hpp>

#include <string_view>

namespace hugin {

inline auto count_components_of_type(
    nlohmann::json const &snapshot, std::string_view type) -> std::size_t
{
    auto count = std::size_t{0};

    if (snapshot.value("type", "") == type)
    {
        ++count;
    }

    for (auto const &child : snapshot.value("subcomponents", nlohmann::json::array()))
    {
        count += count_components_of_type(child, type);
    }

    return count;
}

inline auto find_focused_leaf(nlohmann::json const &snapshot)
    -> nlohmann::json const *
{
    if (!snapshot.value("has_focus", false))
    {
        return nullptr;
    }

    auto const subcomponents = snapshot.find("subcomponents");
    if (subcomponents != snapshot.end())
    {
        for (auto const &child : *subcomponents)
        {
            if (auto const *focused_leaf = find_focused_leaf(child))
            {
                return focused_leaf;
            }
        }
    }

    return &snapshot;
}

inline auto find_component_by_id(
    nlohmann::json const &snapshot, std::string_view id)
    -> nlohmann::json const *
{
    if (snapshot.value("id", "") == id)
    {
        return &snapshot;
    }

    auto const subcomponents = snapshot.find("subcomponents");
    if (subcomponents != snapshot.end())
    {
        for (auto const &child : *subcomponents)
        {
            if (auto const *component = find_component_by_id(child, id))
            {
                return component;
            }
        }
    }

    return nullptr;
}

}  // namespace hugin
