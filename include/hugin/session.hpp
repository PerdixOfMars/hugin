#pragma once

#include <hugin/selectors.hpp>
#include <munin/window.hpp>
#include <nlohmann/json.hpp>
#include <terminalpp/mouse.hpp>
#include <terminalpp/point.hpp>

#include <format>
#include <functional>
#include <stdexcept>
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

class diagnostic_error : public std::runtime_error
{
public:
    explicit diagnostic_error(std::string const &message)
      : std::runtime_error(message)
    {
    }
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

        if (matches.size() > 1U)
        {
            throw diagnostic_error{std::format(
                "{} expected one, found {}; matches: {} \"{}\"",
                selector.describe(),
                matches.size(),
                matches.front().role(),
                matches.front().name())};
        }

        auto const visible_nodes = visible_nodes_for_missing(selector);
        auto message = std::format(
            "{} not found; visible nodes: {}",
            selector.describe(),
            node_summary(visible_nodes.front()));

        for (auto remaining_visible = visible_nodes.begin() + 1;
             remaining_visible != visible_nodes.end();
             ++remaining_visible)
        {
            append_node_summary(message, *remaining_visible);
        }

        throw diagnostic_error{std::move(message)};
    }

private:
    [[nodiscard]] auto content_node() const -> node
    {
        return make_node(content_snapshot());
    }

    [[nodiscard]] auto content_snapshot() const -> nlohmann::json
    {
        return window_.to_json().at("content");
    }

    template <strict_find_selector Selector>
    [[nodiscard]] auto visible_nodes_for_missing(Selector const &selector) const
        -> std::vector<node>
    {
        auto visible_nodes = diagnostic_nodes(selector);
        if (visible_nodes.empty())
        {
            visible_nodes.push_back(content_node());
        }

        return visible_nodes;
    }

    static void append_node_summary(std::string &message, node const &visible)
    {
        message += std::format(", {}", node_summary(visible));
    }

    [[nodiscard]] static auto node_summary(node const &visible) -> std::string
    {
        return std::format("{} \"{}\"", visible.role(), visible.name());
    }

    template <strict_find_selector Selector>
    [[nodiscard]] auto diagnostic_nodes(Selector const &selector) const
        -> std::vector<node>
    {
        auto nodes = std::vector<node>{};
        append_diagnostic_nodes(
            nodes, content_snapshot(), selector, diagnostic_relevance::related);
        append_diagnostic_nodes(
            nodes, content_snapshot(), selector, diagnostic_relevance::context);
        return nodes;
    }

    template <strict_find_selector Selector>
    void append_diagnostic_nodes(
        std::vector<node> &nodes,
        nlohmann::json const &snapshot,
        Selector const &selector,
        diagnostic_relevance relevance) const
    {
        if (selector.relevance(snapshot) == relevance)
        {
            nodes.push_back(make_node(snapshot));
        }

        for (auto const &child :
             snapshot.value("subcomponents", nlohmann::json::array()))
        {
            append_diagnostic_nodes(nodes, child, selector, relevance);
        }
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
