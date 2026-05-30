#pragma once

#include <nlohmann/json.hpp>

#include <concepts>
#include <string>
#include <string_view>

namespace hugin {
namespace detail {

enum class diagnostic_relevance
{
    ignore,
    context,
    related
};

[[nodiscard]] auto has_diagnostic_summary(nlohmann::json const &snapshot)
    -> bool;

struct role_selector
{
    std::string role;

    [[nodiscard]] auto matches(nlohmann::json const &snapshot) const -> bool;
};

struct role_name_selector
{
    std::string role;
    std::string name;

    [[nodiscard]] auto matches(nlohmann::json const &snapshot) const -> bool;

    [[nodiscard]] auto describe() const -> std::string;

    [[nodiscard]] auto relevance(nlohmann::json const &snapshot) const
        -> diagnostic_relevance;
};

struct id_selector
{
    std::string id;

    [[nodiscard]] auto matches(nlohmann::json const &snapshot) const -> bool;

    [[nodiscard]] auto describe() const -> std::string;

    [[nodiscard]] auto relevance(nlohmann::json const &snapshot) const
        -> diagnostic_relevance;
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

}  // namespace detail

namespace by {

auto role(std::string_view role) -> detail::role_selector;

auto role_name(std::string_view role, std::string_view name)
    -> detail::role_name_selector;

auto id(std::string_view id) -> detail::id_selector;

}  // namespace by

}  // namespace hugin
