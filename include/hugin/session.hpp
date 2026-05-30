#pragma once

#include <hugin/diagnostics.hpp>
#include <hugin/selectors.hpp>
#include <nlohmann/json.hpp>
#include <terminalpp/point.hpp>

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace munin {
class window;
}

namespace hugin {

class node
{
public:
    using click_function = std::function<void(terminalpp::point const &)>;

    explicit node(nlohmann::json snapshot, click_function click = {});

    [[nodiscard]] auto role() const -> std::string;

    [[nodiscard]] auto name() const -> std::string;

    void click() const;

private:
    [[nodiscard]] auto click_position() const -> terminalpp::point;

    nlohmann::json snapshot_;
    click_function click_;
};

class session
{
public:
    explicit session(munin::window &window);

    template <detail::snapshot_selector Selector>
    [[nodiscard]] auto query(Selector const &selector) const
        -> std::vector<node>
    {
        auto matches = std::vector<node>{};
        append_matching_descendants(matches, content_snapshot(), selector);
        return matches;
    }

    template <detail::strict_find_selector Selector>
    [[nodiscard]] auto find(Selector const &selector) const -> node
    {
        auto const matches = query(selector);

        if (matches.size() == 1U)
        {
            return matches.front();
        }

        throw diagnostic_error{detail::strict_find_failure_message(
            selector,
            matches,
            content_snapshot(),
            [this](nlohmann::json snapshot) {
                return make_node(std::move(snapshot));
            })};
    }

private:
    [[nodiscard]] auto content_snapshot() const -> nlohmann::json;

    template <typename Predicate>
    void append_matching_descendants(
        std::vector<node> &matches,
        nlohmann::json const &snapshot,
        Predicate const &selector) const
    {
        if (selector.matches(snapshot))
        {
            matches.push_back(make_node(snapshot));
        }

        for (auto const &child :
             snapshot.value("subcomponents", nlohmann::json::array()))
        {
            append_matching_descendants(matches, child, selector);
        }
    }

    [[nodiscard]] auto make_node(nlohmann::json snapshot) const -> node;

    void click_at(terminalpp::point const &position) const;

    munin::window &window_;
};

}  // namespace hugin
