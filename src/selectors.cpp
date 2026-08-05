#include <hugin/selectors.hpp>

#include <format>

namespace hugin {
namespace detail {

auto has_diagnostic_summary(nlohmann::json const &snapshot) -> bool
{
    return !snapshot.value("type", "").empty()
        && !snapshot.value("name", "").empty();
}

auto role_selector::matches(nlohmann::json const &snapshot) const -> bool
{
    return snapshot.value("type", "") == role;
}

auto role_name_selector::matches(nlohmann::json const &snapshot) const -> bool
{
    return snapshot.value("type", "") == role
        && snapshot.value("name", "") == name;
}

auto role_name_selector::describe() const -> std::string
{
    return std::format("role_name({}, {})", role, name);
}

auto role_name_selector::relevance(nlohmann::json const &snapshot) const
    -> diagnostic_relevance
{
    if (!has_diagnostic_summary(snapshot))
    {
        return diagnostic_relevance::ignore;
    }

    return snapshot.value("type", "") == role ? diagnostic_relevance::related
                                              : diagnostic_relevance::context;
}

auto id_selector::matches(nlohmann::json const &snapshot) const -> bool
{
    return snapshot.value("id", "") == id;
}

auto id_selector::describe() const -> std::string
{
    return std::format("id({})", id);
}

auto id_selector::relevance(nlohmann::json const &snapshot) const
    -> diagnostic_relevance
{
    return has_diagnostic_summary(snapshot) ? diagnostic_relevance::context
                                            : diagnostic_relevance::ignore;
}

}  // namespace detail

namespace by {

auto role(std::string_view role) -> detail::role_selector
{
    return detail::role_selector{std::string{role}};
}

auto role_name(std::string_view role, std::string_view name)
    -> detail::role_name_selector
{
    return detail::role_name_selector{std::string{role}, std::string{name}};
}

auto id(std::string_view id) -> detail::id_selector
{
    return detail::id_selector{std::string{id}};
}

}  // namespace by

}  // namespace hugin
