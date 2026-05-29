#pragma once

#include <nlohmann/json.hpp>

#include <concepts>
#include <format>
#include <string>
#include <string_view>

namespace hugin {

enum class diagnostic_relevance
{
    ignore,
    context,
    related
};

[[nodiscard]] inline auto has_diagnostic_summary(nlohmann::json const &snapshot)
    -> bool
{
    return !snapshot.value("type", "").empty()
        && !snapshot.value("name", "").empty();
}

struct role_selector
{
    std::string role;

    [[nodiscard]] auto matches(nlohmann::json const &snapshot) const -> bool
    {
        return snapshot.value("type", "") == role;
    }
};

struct role_name_selector
{
    std::string role;
    std::string name;

    [[nodiscard]] auto matches(nlohmann::json const &snapshot) const -> bool
    {
        return snapshot.value("type", "") == role
            && snapshot.value("name", "") == name;
    }

    [[nodiscard]] auto describe() const -> std::string
    {
        return std::format("role_name({}, {})", role, name);
    }

    [[nodiscard]] auto relevance(nlohmann::json const &snapshot) const
        -> diagnostic_relevance
    {
        if (!has_diagnostic_summary(snapshot))
        {
            return diagnostic_relevance::ignore;
        }

        return snapshot.value("type", "") == role
                 ? diagnostic_relevance::related
                 : diagnostic_relevance::context;
    }
};

struct id_selector
{
    std::string id;

    [[nodiscard]] auto matches(nlohmann::json const &snapshot) const -> bool
    {
        return snapshot.value("id", "") == id;
    }

    [[nodiscard]] auto describe() const -> std::string
    {
        return std::format("id({})", id);
    }

    [[nodiscard]] auto relevance(nlohmann::json const &snapshot) const
        -> diagnostic_relevance
    {
        return has_diagnostic_summary(snapshot) ? diagnostic_relevance::context
                                                : diagnostic_relevance::ignore;
    }
};

template <typename Selector>
concept snapshot_selector =
    requires(Selector const &selector, nlohmann::json const &snapshot) {
        { selector.matches(snapshot) } -> std::same_as<bool>;
    };

template <typename Selector>
concept strict_find_selector =
    snapshot_selector<Selector> && requires(Selector const &selector) {
        { selector.describe() } -> std::convertible_to<std::string>;
    } && requires(Selector const &selector, nlohmann::json const &snapshot) {
        { selector.relevance(snapshot) } -> std::same_as<diagnostic_relevance>;
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

}  // namespace hugin
