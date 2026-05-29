#include "hugin/snapshot_queries.hpp"

namespace hugin {

auto count_components_of_type(
    nlohmann::json const &snapshot, std::string_view type) -> std::size_t
{
    auto count = std::size_t{0};

    if (snapshot.value("type", "") == type)
    {
        ++count;
    }

    for (auto const &child : snapshot.value(
             "subcomponents", nlohmann::json::array()))
    {
        count += count_components_of_type(child, type);
    }

    return count;
}

auto find_focused_leaf(nlohmann::json const &snapshot)
    -> std::optional<nlohmann::json>
{
    if (!snapshot.value("has_focus", false))
    {
        return std::nullopt;
    }

    auto const subcomponents = snapshot.find("subcomponents");
    if (subcomponents != snapshot.end())
    {
        for (auto const &child : *subcomponents)
        {
            if (auto focused_leaf = find_focused_leaf(child))
            {
                return focused_leaf;
            }
        }
    }

    return std::optional<nlohmann::json>{snapshot};
}

auto find_component_by_id(
    nlohmann::json const &snapshot, std::string_view id)
    -> std::optional<nlohmann::json>
{
    if (snapshot.value("id", "") == id)
    {
        return std::optional<nlohmann::json>{snapshot};
    }

    auto const subcomponents = snapshot.find("subcomponents");
    if (subcomponents != snapshot.end())
    {
        for (auto const &child : *subcomponents)
        {
            if (auto component = find_component_by_id(child, id))
            {
                return component;
            }
        }
    }

    return std::nullopt;
}

}  // namespace hugin
