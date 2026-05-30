#include <hugin/selectors.hpp>

#include <format>

namespace hugin {

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

namespace by {

auto role(std::string_view role) -> role_selector
{
    return role_selector{std::string{role}};
}

auto role_name(std::string_view role, std::string_view name)
    -> role_name_selector
{
    return role_name_selector{std::string{role}, std::string{name}};
}

auto id(std::string_view id) -> id_selector
{
    return id_selector{std::string{id}};
}

}  // namespace by

}  // namespace hugin
