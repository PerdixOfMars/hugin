#pragma once

#include <hugin/diagnostics.hpp>
#include <hugin/selectors.hpp>
#include <munin/window.hpp>
#include <nlohmann/json.hpp>
#include <terminalpp/mouse.hpp>
#include <terminalpp/point.hpp>

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace hugin {

class node
{
public:
    using click_function = std::function<void(terminalpp::point const &)>;

    explicit node(nlohmann::json snapshot, click_function click = {})
      : snapshot_(std::move(snapshot)), click_(std::move(click))
    {
    }

    [[nodiscard]] auto role() const -> std::string
    {
        return snapshot_.value("type", "");
    }

    [[nodiscard]] auto name() const -> std::string
    {
        return snapshot_.value("name", "");
    }

    void click() const
    {
        if (click_)
        {
            click_(click_position());
        }
    }

private:
    [[nodiscard]] auto click_position() const -> terminalpp::point
    {
        return {
            snapshot_.at("position").at("x").get<terminalpp::coordinate_type>(),
            snapshot_.at("position")
                .at("y")
                .get<terminalpp::coordinate_type>()};
    }

    nlohmann::json snapshot_;
    click_function click_;
};

class session
{
public:
    explicit session(munin::window &window) : window_(window)
    {
    }

    template <snapshot_selector Selector>
    [[nodiscard]] auto query(Selector const &selector) const
        -> std::vector<node>
    {
        auto matches = std::vector<node>{};
        append_matching_descendants(matches, content_snapshot(), selector);
        return matches;
    }

    template <strict_find_selector Selector>
    [[nodiscard]] auto find(Selector const &selector) const -> node
    {
        auto const matches = query(selector);

        if (matches.size() == 1U)
        {
            return matches.front();
        }

        throw diagnostic_error{strict_find_failure_message(
            selector,
            matches,
            content_snapshot(),
            [this](nlohmann::json snapshot) {
                return make_node(std::move(snapshot));
            })};
    }

private:
    [[nodiscard]] auto content_snapshot() const -> nlohmann::json
    {
        return window_.to_json().at("content");
    }

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

    [[nodiscard]] auto make_node(nlohmann::json snapshot) const -> node
    {
        return node{
            std::move(snapshot),
            [this](terminalpp::point const &position) { click_at(position); }};
    }

    void click_at(terminalpp::point const &position) const
    {
        window_.event(terminalpp::mouse::event{
            terminalpp::mouse::event_type::left_button_down, position});
    }

    munin::window &window_;
};

}  // namespace hugin
