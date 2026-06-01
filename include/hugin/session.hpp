#pragma once

#include <hugin/diagnostics.hpp>
#include <hugin/selectors.hpp>
#include <nlohmann/json.hpp>
#include <terminalpp/point.hpp>

#include <functional>
#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace munin {
class window;
}

namespace terminalpp {
struct virtual_key;
}

namespace hugin {

//* =========================================================================
/// \brief A matched UI element in a Hugin automation session.
///
/// A node is a lightweight handle to a component snapshot returned by
/// session::query() or session::find().  It exposes the component's role and
/// accessible name, and can perform supported actions such as clicking.
///
/// \par Usage
/// \code
/// hugin::session ui(window);
///
/// auto button = ui.find(hugin::by::role_name("button", "OK"));
/// EXPECT_EQ("button", button.role());
/// EXPECT_EQ("OK", button.name());
///
/// button.click();
/// \endcode
//* =========================================================================
class node
{
public:
    using click_function = std::function<void(terminalpp::point const &)>;

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    explicit node(
        nlohmann::json snapshot,
        click_function click = {},
        terminalpp::point click_position = {});

    //* =====================================================================
    /// \brief Returns the accessibility role of this node.
    //* =====================================================================
    [[nodiscard]] auto role() const -> std::string;

    //* =====================================================================
    /// \brief Returns the accessible name of this node.
    //* =====================================================================
    [[nodiscard]] auto name() const -> std::string;

    //* =====================================================================
    /// \brief Returns this node's raw Introspection JSON snapshot.
    //* =====================================================================
    [[nodiscard]] auto raw_json() const -> nlohmann::json const &;

    //* =====================================================================
    /// \brief Clicks this node if it has a click action.
    //* =====================================================================
    void click() const;

private:
    [[nodiscard]] auto click_position() const -> terminalpp::point;

    nlohmann::json snapshot_;
    click_function click_;
    terminalpp::point click_position_;
};

//* =========================================================================
/// \brief A query and interaction session for a Munin window.
///
/// A session is the main public entry point for Hugin.  It reads the JSON
/// snapshot exposed by a munin::window, finds nodes using selectors from
/// hugin::by, and routes node actions back to the underlying window.
///
/// Use query() when zero or more matching nodes are expected.  Use find()
/// when exactly one node is expected; it throws hugin::diagnostic_error with
/// visible UI context if no node, or more than one node, matches.
///
/// \par Usage
/// \code
/// hugin::session ui(window);
///
/// auto buttons = ui.query(hugin::by::role("button"));
///
/// auto save = ui.find(hugin::by::role_name("button", "Save"));
/// save.click();
/// \endcode
//* =========================================================================
class session
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    explicit session(munin::window &window);

    //* =====================================================================
    /// \brief Sends one keypress through the native Munin event path.
    //* =====================================================================
    void send_key(terminalpp::virtual_key const &key) const;

    //* =====================================================================
    /// \brief Sends keypresses through the native Munin event path.
    //* =====================================================================
    void send_keys(std::initializer_list<terminalpp::virtual_key> keys) const;

    //* =====================================================================
    /// \brief Sends text as keypresses through the native Munin event path.
    //* =====================================================================
    void send_text(std::string_view text) const;

    //* =====================================================================
    /// \brief Returns all nodes matching the selector.
    //* =====================================================================
    template <detail::snapshot_selector Selector>
    [[nodiscard]] auto query(Selector const &selector) const
        -> std::vector<node>
    {
        auto matches = std::vector<node>{};
        append_matching_descendants(matches, content_snapshot(), selector, {});
        return matches;
    }

    //* =====================================================================
    /// \brief Returns the one node matching the selector, or throws.
    //* =====================================================================
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

    //* =====================================================================
    /// \brief Asserts that the selected node is the focused leaf component.
    //* =====================================================================
    template <detail::strict_find_selector Selector>
    void assert_focused(Selector const &selector) const
    {
        auto const selected = find(selector);
        auto const &snapshot = selected.raw_json();

        if (!snapshot.value("has_focus", false)
            || !snapshot.value("subcomponents", nlohmann::json::array())
                    .empty())
        {
            throw diagnostic_error{
                selector.describe() + " is not focused leaf"};
        }
    }

private:
    [[nodiscard]] auto content_snapshot() const -> nlohmann::json;
    [[nodiscard]] static auto snapshot_position(nlohmann::json const &snapshot)
        -> terminalpp::point;

    template <typename Predicate>
    void append_matching_descendants(
        std::vector<node> &matches,
        nlohmann::json const &snapshot,
        Predicate const &selector,
        terminalpp::point const &parent_position) const
    {
        auto const position = parent_position + snapshot_position(snapshot);

        if (selector.matches(snapshot))
        {
            matches.push_back(make_node(snapshot, position));
        }

        for (auto const &child :
             snapshot.value("subcomponents", nlohmann::json::array()))
        {
            append_matching_descendants(matches, child, selector, position);
        }
    }

    [[nodiscard]] auto make_node(
        nlohmann::json snapshot,
        terminalpp::point click_position = {}) const -> node;

    void click_at(terminalpp::point const &position) const;

    munin::window &window_;
};

}  // namespace hugin
