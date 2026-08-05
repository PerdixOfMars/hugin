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

//* =========================================================================
/// \brief Selects nodes by accessibility role.
///
/// Role selectors are intended for broad queries, such as finding all buttons
/// currently visible in a window.
///
/// \par Usage
/// \code
/// auto buttons = ui.query(hugin::by::role("button"));
/// \endcode
//* =========================================================================
auto role(std::string_view role) -> detail::role_selector;

//* =========================================================================
/// \brief Selects one node by accessibility role and accessible name.
///
/// Role-and-name selectors are intended for strict lookup when the user-facing
/// label identifies the node within its role.
///
/// \par Usage
/// \code
/// auto ok = ui.find(hugin::by::role_name("button", "OK"));
/// ok.click();
/// \endcode
//* =========================================================================
auto role_name(std::string_view role, std::string_view name)
    -> detail::role_name_selector;

//* =========================================================================
/// \brief Selects one node by automation identifier.
///
/// Automation identifiers are useful when visible text is not stable or is not
/// sufficient to distinguish the target node.
///
/// \par Usage
/// \code
/// auto action = ui.find(hugin::by::id("primary_action"));
/// action.click();
/// \endcode
//* =========================================================================
auto id(std::string_view id) -> detail::id_selector;

}  // namespace by

}  // namespace hugin
