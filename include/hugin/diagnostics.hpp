#pragma once

#include <hugin/selectors.hpp>
#include <nlohmann/json.hpp>

#include <format>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace hugin {

class diagnostic_error : public std::runtime_error
{
public:
    explicit diagnostic_error(std::string const &message);
};

namespace detail {

template <typename Node>
[[nodiscard]] auto node_summary(Node const &visible) -> std::string
{
    return std::format("{} \"{}\"", visible.role(), visible.name());
}

template <typename Node>
void append_node_summary(std::string &message, Node const &visible)
{
    message += std::format(", {}", node_summary(visible));
}

template <typename MakeNode>
using diagnostic_node_type = std::invoke_result_t<MakeNode, nlohmann::json>;

template <strict_find_selector Selector, typename MakeNode>
void append_diagnostic_nodes(
    std::vector<diagnostic_node_type<MakeNode>> &nodes,
    nlohmann::json const &snapshot,
    Selector const &selector,
    diagnostic_relevance relevance,
    MakeNode const &make_node)
{
    if (selector.relevance(snapshot) == relevance)
    {
        nodes.push_back(make_node(snapshot));
    }

    for (auto const &child :
         snapshot.value("subcomponents", nlohmann::json::array()))
    {
        append_diagnostic_nodes(nodes, child, selector, relevance, make_node);
    }
}

template <strict_find_selector Selector, typename MakeNode>
[[nodiscard]] auto diagnostic_nodes(
    nlohmann::json const &content,
    Selector const &selector,
    MakeNode const &make_node) -> std::vector<diagnostic_node_type<MakeNode>>
{
    auto nodes = std::vector<diagnostic_node_type<MakeNode>>{};
    append_diagnostic_nodes(
        nodes, content, selector, diagnostic_relevance::related, make_node);
    append_diagnostic_nodes(
        nodes, content, selector, diagnostic_relevance::context, make_node);
    return nodes;
}

template <strict_find_selector Selector, typename Node, typename MakeNode>
[[nodiscard]] auto strict_find_failure_message(
    Selector const &selector,
    std::vector<Node> const &matches,
    nlohmann::json const &content,
    MakeNode const &make_node) -> std::string
{
    if (matches.size() > 1U)
    {
        return std::format(
            "{} expected one, found {}; matches: {} \"{}\"",
            selector.describe(),
            matches.size(),
            matches.front().role(),
            matches.front().name());
    }

    auto visible_nodes = diagnostic_nodes(content, selector, make_node);
    if (visible_nodes.empty())
    {
        visible_nodes.push_back(make_node(content));
    }

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

    return message;
}

}  // namespace detail

}  // namespace hugin
