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

}  // namespace hugin
